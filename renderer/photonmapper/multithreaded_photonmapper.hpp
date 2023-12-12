//  
// multithreaded_photonmapper.hpp
//
// Description:
//  Non-BVH Photonmapper runner
//
// Authors:
//  Samuel García
//  Laura González
//
// Date:
//  10/2022.
//

#ifndef INFORMATICA_GRAFICA_PHOTONMAPPER_RENDERER_HPP
#define INFORMATICA_GRAFICA_PHOTONMAPPER_RENDERER_HPP

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

#ifdef benchmarking
#include "benchmarking.hpp"
#endif


#define MAX_WALKS 750000
#define MAX_PHOTONS 100000

/*
 *
 * Photon scattering
 *
 */
struct RenderingMessagePhotonScattering {
    bool stop = false;
    Ray ray;
    std::shared_ptr<ThreadSafeCounter> photons_stored;
};

std::vector<Photon> rendering_job_photonmapper(const Scene &scene, const Ray &ray, PhotonmappingDirectLightMethod method) {
    std::vector<Photon> photons;
    scatter_photons(scene, ray, 0, photons, Vector3d(1, 1, 1), method);

    return photons;
}

void rendering_thread_photonmapper(const Scene &scene, Channel<RenderingMessagePhotonScattering> &job_channel, Channel<std::vector<Photon>> &job_done_channel, PhotonmappingDirectLightMethod method) {
    while(true) {
        RenderingMessagePhotonScattering job = job_channel.receive();

        if (job.stop) {
            break;
        } else {
            if (job.photons_stored->get() < MAX_PHOTONS) {
                auto photons = rendering_job_photonmapper(scene, job.ray, method);
                job.photons_stored->increment(photons.size());
                job_done_channel.send(std::move(photons));
            } else {
                job_done_channel.send(std::vector<Photon>());
            }
        }
    }
}

void stop_rendering_jobs_photonmapper(std::chrono::high_resolution_clock::time_point &timer,
                                      Channel<RenderingMessagePhotonScattering> &job_channel, std::vector<std::thread> &threads,
                                      const unsigned int processor_count) { // Parar threads
    for (size_t i = 0; i < processor_count; i++) {
        job_channel.send(RenderingMessagePhotonScattering{true, Ray(), nullptr});
    }

    for (size_t i = 0; i < processor_count; i++) {
        threads[i].join();
    }

    std::cout << "Photon scattering total time: " << time_elapsed(timer) << std::endl;
#ifdef benchmarking
    Benchmarking::print_statistics();
#endif
}

std::pair<std::vector<Photon>, int> track_rendering_jobs_photonmapper(Channel<std::vector<Photon>> &job_done_channel) {
    std::vector<Photon> all_photons;
    all_photons.reserve(MAX_PHOTONS);

    int walks = 0;

    std::cout << "Scattering photons..." << std::endl;
    for (size_t i = 0; i < MAX_WALKS; i++) {
        std::vector<Photon> received_vec = job_done_channel.receive();
        if (received_vec.empty()) ++walks;
        all_photons.insert(all_photons.end(), received_vec.begin(), received_vec.end());
    }

    return std::make_pair(all_photons, walks);
}

void send_rendering_jobs_photonmapper(const Scene &scene, Channel<RenderingMessagePhotonScattering> &job_channel, const std::shared_ptr<ThreadSafeCounter> &photons_stored) {
    PhotonMapperRayGenerator f(scene.point_lights, MAX_WALKS);

    while(true) {
        auto tuple = f.generate_ray();

        if (!tuple.second)  break;
        else {
            job_channel.send(RenderingMessagePhotonScattering{false, tuple.first, photons_stored});
        }
    }
}

nn::KDTree<Photon, 3, PhotonAxisPosition> multithreaded_photon_scattering(const Scene &scene, PhotonmappingDirectLightMethod method) {
    std::shared_ptr<ThreadSafeCounter> photons_stored = std::make_shared<ThreadSafeCounter>();

    auto timer = empezar_timer();

    Channel<RenderingMessagePhotonScattering> job_channel;
    Channel<std::vector<Photon>> job_done_channel;

    std::vector<std::thread> threads;
    const auto processor_count = std::thread::hardware_concurrency();
    for (size_t i = 0; i < processor_count; i++) {
        threads.emplace_back(std::thread(rendering_thread_photonmapper, std::ref(scene), std::ref(job_channel), std::ref(job_done_channel), method));
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
void rendering_job_photonmapper_renderer(const Scene &scene, const nn::KDTree<Photon, 3, PhotonAxisPosition> &photons, std::vector<Vector3d> &img, size_t i, size_t j, PhotonmappingKernel kernel, PhotonmappingDirectLightMethod method) {
    Vector3d temp_emission;

    for (size_t r = 0; r < scene.camera.rays_per_pixel; r++) {
        Ray ray = scene.camera.get_ray(i, j);

        temp_emission = temp_emission + integrator_sample_photonmapping(scene, photons, ray, kernel, method);
    }

    img[i*scene.camera.width + j] = temp_emission / scene.camera.rays_per_pixel;
}

void rendering_thread_photonmapper_renderer(const Scene &scene, const nn::KDTree<Photon, 3, PhotonAxisPosition> &photons, std::vector<Vector3d> &img, Channel<RenderingMessage> &job_channel, Channel<int> &job_done_channel, PhotonmappingKernel kernel, PhotonmappingDirectLightMethod method) {
    while(true) {
        RenderingMessage job = job_channel.receive();

        if (job.stop) {
            break;
        } else {
            rendering_job_photonmapper_renderer(scene, photons, img, job.i, job.j, kernel, method);
            job_done_channel.send(0);
        }
    }
}

void render_multithreaded_photonmapper(const Scene &scene, PhotonmappingKernel kernel, PhotonmappingDirectLightMethod method) {
    auto photons = multithreaded_photon_scattering(scene, method);

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
        threads.emplace_back(std::thread(rendering_thread_photonmapper_renderer, std::ref(scene), std::ref(photons), std::ref(img), std::ref(job_channel), std::ref(job_done_channel), kernel, method));
    }
    std::cout << processor_count << " rendering threads started..." << std::endl;

    send_rendering_jobs(scene, job_channel);
    track_rendering_jobs(scene, job_done_channel);
    stop_rendering_jobs(scene, timer, job_channel, img, threads, processor_count);
}

#endif //INFORMATICA_GRAFICA_PHOTONMAPPER_RENDERER_HPP
