//  
// renderer.hpp
// 
// Description:
//  Rendering runner auxiliary functions
// 
//
// Authors:
//  Samuel García
//  Laura González
//
// Date:
//  10/2022.
//

#ifndef INFORMATICA_GRAFICA_RENDERER_HPP
#define INFORMATICA_GRAFICA_RENDERER_HPP

#include <memory>
#include <array>
#include "camera.hpp"
#include "Figure.hpp"
#include "../lib/Ppm.hpp"
#include "../lib/tonemapper.hpp"
#include "Channel.hpp"
#include "../lib/Scene.hpp"
#ifdef benchmarking
#include "benchmarking.hpp"
#endif

struct RenderingMessage {
    bool stop = false;
    size_t i = 0;
    size_t j = 0;
};

void launch_command(std::string command) {
    system(command.c_str());
}

void execute_convert(std::string filename) {
    std::string command = "convert '" + filename + ".ppm'" + " '" + filename + ".png'";
    std::thread worker (launch_command, command);
    worker.join();
}

void write_results(const std::vector<Vector3d> &img, const Camera &camera) {
    Ppm ppm(img, camera.width, camera.height);
    ppm.write_hdr("render_hdr.ppm");
    (ToneMapper::equalize(ppm)).write_ldr("render_ldr_equalized.ppm");

    std::array<double, 7> clamp_values{0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0};
    std::array<double, 11> gamma_values{0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0, 1.5, 2.0, 2.2, 2.4};

    ppm.read("render_hdr.ppm");
    for (double cv : clamp_values) {
        for (double gv : gamma_values) {
            std::string filename = "tonemapping/render_hdr_tonemapped_clamp_" + std::to_string(cv) + "_gamma_"+ std::to_string(gv);
            ToneMapper::clamp_and_gamma_curve(ppm, ppm.max_rgb_value() * cv, gv).write(filename + ".ppm");
            execute_convert(filename);
            std::remove((filename + ".ppm").c_str());
        }
    }
}

void stop_rendering_jobs(const Scene &scene,
                         std::chrono::high_resolution_clock::time_point &timer,
                         Channel<RenderingMessage> &job_channel,
                         const std::vector<Vector3d> &img,
                         std::vector<std::thread> &threads,
                         const unsigned int processor_count) {
    for (size_t i = 0; i < processor_count; i++) {
        job_channel.send(RenderingMessage{true, 0, 0});
    }

    for (size_t i = 0; i < processor_count; i++) {
        threads[i].join();
    }

    std::cout << std::endl << "Rendering time: " << time_elapsed(timer) << std::endl;
#ifdef benchmarking
    Benchmarking::print_statistics();
#endif
    std::cout << std::endl << "Tonemapping results..." << std::endl,
    write_results(img, scene.camera);
}

void track_rendering_jobs(const Scene &scene, Channel<int> &job_done_channel) {
    unsigned long total_pixels = scene.camera.width * scene.camera.height;
    unsigned long pixels_written = 0;
    unsigned long pixels_per_message = total_pixels/100;
    unsigned long pixels_per_message_count = 0;
    unsigned long progress_messages_sent = 0;
    auto start = std::chrono::high_resolution_clock::now();
    std::vector<std::chrono::milliseconds> durations;
#ifdef benchmarking
    Benchmarking::light_rays_per_median_time = pixels_per_message * scene.camera.rays_per_pixel;
#endif
    for (size_t i = 0; i < total_pixels; i++) {
        job_done_channel.receive();
        ++pixels_written;

        if (++pixels_per_message_count >= pixels_per_message) {
            auto elapsed_time = remaining_time(start, progress_messages_sent, durations);

            double percent = ((double) pixels_written / (double) total_pixels) * 100.0;
            std::cout << '\r' << pixels_written << "/" << total_pixels << " pixels written (" << percent << "%) " << "Remaining time estimation: " << elapsed_time;
            std::flush(std::cout);

            pixels_per_message_count = 0;
            ++progress_messages_sent;

            start = std::chrono::high_resolution_clock::now();
        }
    }

    auto median_duration = std::chrono::milliseconds(0);
    for (auto d : durations) {
        median_duration += d;
    }
#ifdef benchmarking
    Benchmarking::median_time_for_n_rays = median_duration / durations.size();
#endif
}

void send_rendering_jobs(const Scene &scene, Channel<RenderingMessage> &job_channel) {
    for (size_t i = 0; i < scene.camera.height; i++) {
        for (size_t j = 0; j < scene.camera.width; j++) {
            job_channel.send(RenderingMessage{false, i, j});
        }
    }
}

#endif //INFORMATICA_GRAFICA_RENDERER_HPP
