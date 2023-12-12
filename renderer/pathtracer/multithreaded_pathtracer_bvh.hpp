//  
// multithreaded_pathtracer_bvh.hpp
//
// Description:
//  BVH Pathtracer runner
//
// Authors:
//  Samuel García
//  Laura González
//
// Date:
//  10/2022.
//

#ifndef INFORMATICA_GRAFICA_MULTITHREADED_PATHTRACER_BVH_HPP
#define INFORMATICA_GRAFICA_MULTITHREADED_PATHTRACER_BVH_HPP

#include <memory>
#include "camera.hpp"
#include "Figure.hpp"
#include "../../lib/Ppm.hpp"
#include "../../lib/tonemapper.hpp"
#include "Channel.hpp"
#include "renderer.hpp"
#include "BVH.hpp"
#include "pathtracing.hpp"

#ifdef benchmarking
#include "benchmarking.hpp"
#endif


void rendering_job_bvh(const Scene &scene, const BVH &bvh_tree, std::vector<Vector3d> &img, int i, int j) {
    Vector3d temp_emission;

    size_t number_of_bounces = 0;
    for (size_t r = 0; r < scene.camera.rays_per_pixel; r++) {
        Ray ray = scene.camera.get_ray(i, j);

        temp_emission = temp_emission + integrator_sample_bvh(scene, bvh_tree, ray, number_of_bounces);
    }

    img[i*scene.camera.width + j] = temp_emission / (double)scene.camera.rays_per_pixel;
}

void rendering_thread_bvh(const Scene &scene,
                          const BVH &bvh_tree,
                          std::vector<Vector3d> &img,
                          Channel<RenderingMessage> &job_channel,
                          Channel<int> &job_done_channel) {
    while(true) {
        RenderingMessage job = job_channel.receive();

        if (job.stop) {
            break;
        } else {
            rendering_job_bvh(scene, bvh_tree, img, job.i, job.j);
            job_done_channel.send(0);
        }
    }
}

void render_multithreaded_bvh(Scene &scene, BvhMethod method) {
    if (method == SORT) std::cout << "Building BVH tree for the scene with the sorting strategy..." << std::endl;
    else if (method == CENTROID) std::cout << "Building BVH tree for the scene with the centroid strategy..." << std::endl;

    auto timer = empezar_timer();

    Channel<RenderingMessage> job_channel;
    Channel<int> job_done_channel;

    std::vector<Vector3d> img;
    img.reserve(scene.camera.height * scene.camera.width);
    for (size_t i = 0; i < scene.camera.height * scene.camera.width; i++)
        img.emplace_back(Vector3d());

    BVH bvh_tree(scene, method);
    scene.figures.clear(); // We have moved all figures to the tree

    std::vector<std::thread> threads;
    const auto processor_count = std::thread::hardware_concurrency();
    for (size_t i = 0; i < processor_count; i++) {
        threads.emplace_back(std::thread(rendering_thread_bvh, std::ref(scene), std::ref(bvh_tree), std::ref(img), std::ref(job_channel), std::ref(job_done_channel)));
    }
    std::cout << processor_count << " rendering threads started..." << std::endl;

    send_rendering_jobs(scene, job_channel);
    track_rendering_jobs(scene, job_done_channel);
    stop_rendering_jobs(scene, timer, job_channel, img, threads, processor_count);
}



#endif //INFORMATICA_GRAFICA_MULTITHREADED_PATHTRACER_BVH_HPP
