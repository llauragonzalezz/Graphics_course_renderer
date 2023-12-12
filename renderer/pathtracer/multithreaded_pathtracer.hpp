//  
// multithreaded_pathtracer.hpp
//
// Description:
//  Non-BVH Pathtracer runner
//
//
// Authors:
//  Samuel García
//  Laura González
//
// Date:
//  10/2022.
//

#ifndef INFORMATICA_GRAFICA_MULTITHREADED_PATHTRACER_HPP
#define INFORMATICA_GRAFICA_MULTITHREADED_PATHTRACER_HPP

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
#ifdef benchmarking
#include "benchmarking.hpp"
#endif

void rendering_job(const Scene &scene, std::vector<Vector3d> &img, size_t i, size_t j) {
    Vector3d temp_emission;

    size_t number_of_bounces = 0;
    for (size_t r = 0; r < scene.camera.rays_per_pixel; r++) {
        Ray ray = scene.camera.get_ray(i, j);

        temp_emission = temp_emission + integrator_sample(scene, ray, number_of_bounces);
    }

    img[i*scene.camera.width + j] = temp_emission / scene.camera.rays_per_pixel;
}

void rendering_thread(const Scene &scene,
                      std::vector<Vector3d> &img,
                      Channel<RenderingMessage> &job_channel,
                      Channel<int> &job_done_channel) {
    while(true) {
        RenderingMessage job = job_channel.receive();

        if (job.stop) {
            break;
        } else {
            rendering_job(scene, img, job.i, job.j);
            job_done_channel.send(0);
        }
    }
}

void render_multithreaded(const Scene &scene) {
    auto timer = empezar_timer();

    Channel<RenderingMessage> job_channel;
    Channel<int> job_done_channel;

    // Prealojar memoria para el resultado
    std::vector<Vector3d> img;
    img.reserve(scene.camera.height * scene.camera.width);
    for (size_t i = 0; i < scene.camera.height * scene.camera.width; i++)
        img.emplace_back(Vector3d());

    std::vector<std::thread> threads;
    const auto processor_count = std::thread::hardware_concurrency();
    for (size_t i = 0; i < processor_count; i++) {
        threads.emplace_back(std::thread(rendering_thread, std::ref(scene), std::ref(img), std::ref(job_channel), std::ref(job_done_channel)));
    }
    std::cout << processor_count << " rendering threads started..." << std::endl;

    send_rendering_jobs(scene, job_channel);
    track_rendering_jobs(scene, job_done_channel);
    stop_rendering_jobs(scene, timer, job_channel, img, threads, processor_count);
}

#endif //INFORMATICA_GRAFICA_MULTITHREADED_PATHTRACER_HPP
