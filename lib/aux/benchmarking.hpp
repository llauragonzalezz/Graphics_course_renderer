//  
// benchmarking.hpp
// 
// Description:
//  Benchmarking class for comparing different acceleration structures
// 
//
// Authors:
//  Samuel García
//  Laura González
//
// Date:
//  11/2022.
//

#ifndef INFORMATICA_GRAFICA_BENCHMARKING_HPP
#define INFORMATICA_GRAFICA_BENCHMARKING_HPP

#include <iostream>
#include <chrono>
#include <vector>
#include <sstream>
#include <mutex>

namespace Benchmarking {
    inline std::mutex mutex;
    inline unsigned long light_rays_traced = 0;
    inline unsigned long figures_checked = 0;
    inline unsigned long bounds_checked = 0;
    inline std::chrono::milliseconds median_time_for_n_rays = std::chrono::milliseconds(0);
    inline unsigned long light_rays_per_median_time = 0;

    void print_statistics();
    void count_ray_traced();
    void count_figure_checked();
    void count_bounds_checked();
}

#endif //INFORMATICA_GRAFICA_BENCHMARKING_HPP
