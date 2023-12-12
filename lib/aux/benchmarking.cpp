//  
// benchmarking.cpp
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


#include "benchmarking.hpp"

void Benchmarking::print_statistics() {
    std::cout << "Statistics:" << std::endl;
    std::cout << "\tLight rays traced: " << Benchmarking::light_rays_traced << std::endl;
    std::cout << "\tFigures tested for collision: " << Benchmarking::figures_checked << std::endl;
    std::cout << "\tBounds tested for collision: " << Benchmarking::bounds_checked << std::endl;

    auto secs = std::chrono::duration_cast<std::chrono::seconds>(Benchmarking::median_time_for_n_rays);
    Benchmarking::median_time_for_n_rays -= std::chrono::duration_cast<std::chrono::milliseconds>(secs);

    auto mins = std::chrono::duration_cast<std::chrono::minutes>(secs);
    secs -= std::chrono::duration_cast<std::chrono::seconds>(mins);

    auto hour = std::chrono::duration_cast<std::chrono::hours>(mins);
    mins -= std::chrono::duration_cast<std::chrono::minutes>(hour);

    std::stringstream ss;
    ss << hour.count() << " h : " << mins.count() << " m : " << secs.count() << " s : " << Benchmarking::median_time_for_n_rays.count() << " ms";

    std::cout << "\tMedian time per " << Benchmarking::light_rays_per_median_time << " light rays casted (1% of total): " <<  ss.str() << std::endl;
}

void  Benchmarking::count_ray_traced() {
    std::unique_lock<std::mutex> lock(Benchmarking::mutex);

    ++Benchmarking::light_rays_traced;
}

void  Benchmarking::count_figure_checked() {
    std::unique_lock<std::mutex> lock(Benchmarking::mutex);

    ++Benchmarking::figures_checked;
}

void  Benchmarking::count_bounds_checked() {
    std::unique_lock<std::mutex> lock(Benchmarking::mutex);

    ++Benchmarking::bounds_checked;
}