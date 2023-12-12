//  
// multithreaded_photonmapper_bvh.hpp
//
// Description:
//  BVH Photonmapper runner
//
//
// Authors:
//  Samuel García
//  Laura González
//
// Date:
//  10/2022.
//

#ifndef INFORMATICA_GRAFICA_PHOTONMAPPER_RENDERER_BVH_HPP
#define INFORMATICA_GRAFICA_PHOTONMAPPER_RENDERER_BVH_HPP

#include <memory>
#include "camera.hpp"
#include "Figure.hpp"
#include "../../lib/Ppm.hpp"
#include "../../lib/tonemapper.hpp"
#include "Channel.hpp"
#include "../../lib/PointLight.hpp"
#include "../../lib/Scene.hpp"
#include "renderer.hpp"
#include "pathtracing.hpp"
#include "../../lib/Photon.hpp"
#include "../../photonmapping/photonmapping.hpp"
#include "../../photonmapping/photonmapping_kdtree.hpp"
#include "ThreadSafeCounter.hpp"
#include "multithreaded_photonmapper.hpp"

#ifdef benchmarking
#include "benchmarking.hpp"
#endif


#define MAX_WALKS 1000000
#define MAX_PHOTONS 500000

/*
 *
 * Photon scattering
 *
 */
std::vector<Photon> rendering_job_photonmapper_bvh(const Scene &scene, const BVH &bvh_tree, const Ray &ray, PhotonmappingDirectLightMethod method) {
    std::vector<Photon> photons;
    scatter_photons_bvh(scene, bvh_tree, ray, 0, photons, Vector3d(1, 1, 1), method);

    return photons;
}

void rendering_thread_photonmapper_bvh(const Scene &scene, const BVH &bvh_tree, Channel<RenderingMessagePhotonScattering> &job_channel, Channel<std::vector<Photon>> &job_done_channel, PhotonmappingDirectLightMethod method) {
    while(true) {
        RenderingMessagePhotonScattering job = job_channel.receive();

        if (job.stop) {
            break;
        } else {
            if (job.photons_stored->get() < MAX_PHOTONS) {
                auto photons = rendering_job_photonmapper_bvh(scene, bvh_tree, job.ray, method);
                job.photons_stored->increment(photons.size());
                job_done_channel.send(std::move(photons));
            } else {
                job_done_channel.send(std::vector<Photon>());
            }
        }
    }
}

nn::KDTree<Photon, 3, PhotonAxisPosition> multithreaded_photon_scattering_bvh(const Scene &scene, const BVH &bvh_tree, PhotonmappingDirectLightMethod method) {
    std::shared_ptr<ThreadSafeCounter> photons_stored = std::make_shared<ThreadSafeCounter>();

    auto timer = empezar_timer();

    Channel<RenderingMessagePhotonScattering> job_channel;
    Channel<std::vector<Photon>> job_done_channel;

    std::vector<std::thread> threads;
    const auto processor_count = std::thread::hardware_concurrency();
    for (size_t i = 0; i < processor_count; i++) {
        threads.emplace_back(std::thread(rendering_thread_photonmapper_bvh, std::ref(scene), std::ref(bvh_tree), std::ref(job_channel), std::ref(job_done_channel), method));
    }
    std::cout << processor_count << " photon scattering threads started..." << std::endl;

    send_rendering_jobs_photonmapper(scene, job_channel, photons_stored);
    std::pair<std::vector<Photon>, int> result = track_rendering_jobs_photonmapper(job_done_channel);
    stop_rendering_jobs_photonmapper(timer, job_channel, threads, processor_count);

    std::cout << "Scattered photons: " << result.first.size() << std::endl;
    std::cout << "Number of walks: " << result.second << "/" << MAX_WALKS << std::endl << std::endl;

    for (Photon &photon : result.first) {
        photon.flux = 4*M_PI*photon.flux / result.second;
    }

    return {result.first, PhotonAxisPosition()};
}


/*
 *
 * Rendering based on the scattered photons
 *
 */
void rendering_job_photonmapper_renderer_bvh(const Scene &scene, const BVH &bvh_tree, const nn::KDTree<Photon, 3, PhotonAxisPosition> &photons, std::vector<Vector3d> &img, size_t i, size_t j, PhotonmappingKernel kernel, PhotonmappingDirectLightMethod method) {
    Vector3d temp_emission;

    for (size_t r = 0; r < scene.camera.rays_per_pixel; r++) {
        Ray ray = scene.camera.get_ray(i, j);

        temp_emission = temp_emission + integrator_sample_photonmapping_bvh(scene, bvh_tree, photons, ray, kernel, method);
    }

    img[i*scene.camera.width + j] = temp_emission / scene.camera.rays_per_pixel;
}

void rendering_thread_photonmapper_renderer_bvh(const Scene &scene, const BVH &bvh_tree, const nn::KDTree<Photon, 3, PhotonAxisPosition> &photons, std::vector<Vector3d> &img, Channel<RenderingMessage> &job_channel, Channel<int> &job_done_channel, PhotonmappingKernel kernel, PhotonmappingDirectLightMethod method) {
    while(true) {
        RenderingMessage job = job_channel.receive();

        if (job.stop) {
            break;
        } else {
            rendering_job_photonmapper_renderer_bvh(scene, bvh_tree, photons, img, job.i, job.j, kernel, method);
            job_done_channel.send(0);
        }
    }
}

void render_multithreaded_photonmapper_bvh(Scene &scene, PhotonmappingKernel kernel, PhotonmappingDirectLightMethod method, BvhMethod bvh_method) {
    if (bvh_method == SORT) std::cout << "Building BVH tree for the scene with the sorting strategy..." << std::endl;
    else if (bvh_method == CENTROID) std::cout << "Building BVH tree for the scene with the centroid strategy..." << std::endl;

    BVH bvh_tree(scene, bvh_method);
    scene.figures.clear();

    auto photons = multithreaded_photon_scattering_bvh(scene, bvh_tree, method);

    auto timer = empezar_timer();

    Channel<RenderingMessage> job_channel;
    Channel<int> job_done_channel;

    std::vector<Vector3d> img;
    img.reserve(scene.camera.height * scene.camera.width);
    for (size_t i = 0; i < scene.camera.height * scene.camera.width; i++)
        img.emplace_back(Vector3d());

    std::vector<std::thread> threads;
    const auto processor_count = std::thread::hardware_concurrency();
    for (size_t i = 0; i < processor_count; i++) {
        threads.emplace_back(std::thread(rendering_thread_photonmapper_renderer_bvh, std::ref(scene), std::ref(bvh_tree), std::ref(photons), std::ref(img), std::ref(job_channel), std::ref(job_done_channel), kernel, method));
    }
    std::cout << processor_count << " rendering threads started..." << std::endl;

    send_rendering_jobs(scene, job_channel);
    track_rendering_jobs(scene, job_done_channel);
    stop_rendering_jobs(scene, timer, job_channel, img, threads, processor_count);
}

#endif //INFORMATICA_GRAFICA_PHOTONMAPPER_RENDERER_BVH_HPP
