//
// aux.hpp
//
// Description:
//
//  Auxiliary functions
//
// Authors:
//  Samuel García
//  Laura González
//
// Date:
//  10/2022.
//

#pragma once

#ifndef INFORMATICA_GRAFICA_AUX_HPP
#define INFORMATICA_GRAFICA_AUX_HPP

#include <chrono>
#include <sstream>
#include <vector>

#define M_PI           3.14159265358979323846
#define ROUNDING_ERROR 1e-4

// https://stackoverflow.com/questions/17333/what-is-the-most-effective-way-for-float-and-double-comparison
inline bool equals(const double a, const double b) {
    return ((a - b) < ROUNDING_ERROR) && ((b - a) < ROUNDING_ERROR);
}

inline bool greater_than(double a, double b) {
    return a > b + ROUNDING_ERROR;
}

inline bool less_than(double a, double b) {
    return a < b - ROUNDING_ERROR;
}

inline bool greater_or_equal(double a, double b) {
    return a > b - ROUNDING_ERROR;
}

inline bool less_or_equal(double a, double b) {
    return a < b + ROUNDING_ERROR;
}

// Interpola linealmente entre dos valores
inline double lerp(double t, double v1, double v2) {
    return (1 - t) * v1 + t * v2;
}

std::chrono::high_resolution_clock::time_point empezar_timer() {
    return std::chrono::high_resolution_clock::now();
}

// https://stackoverflow.com/questions/50727304/convert-milliseconds-to-hoursminutessecondsmilliseconds-in-c
std::string format_duration(std::chrono::milliseconds ms) {
    auto secs = std::chrono::duration_cast<std::chrono::seconds>(ms);
    ms -= std::chrono::duration_cast<std::chrono::milliseconds>(secs);

    auto mins = std::chrono::duration_cast<std::chrono::minutes>(secs);
    secs -= std::chrono::duration_cast<std::chrono::seconds>(mins);

    auto hour = std::chrono::duration_cast<std::chrono::hours>(mins);
    mins -= std::chrono::duration_cast<std::chrono::minutes>(hour);

    std::stringstream ss;
    ss << hour.count() << " h : " << mins.count() << " m : " << secs.count() << " s : " << ms.count() << " ms";
    return ss.str();
}

std::string remaining_time(std::chrono::high_resolution_clock::time_point &start,
                           unsigned long progress_messages_sent,
                           std::vector<std::chrono::milliseconds> &durations) {
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    durations.emplace_back(duration);
    auto median_duration = std::chrono::milliseconds(0);
    for (auto d : durations) {
        median_duration += d;
    }
    median_duration /= durations.size();

    return format_duration(median_duration * (99-progress_messages_sent));
}

std::string time_elapsed(std::chrono::high_resolution_clock::time_point &start) {
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);

    return format_duration(duration);
}

#endif //INFORMATICA_GRAFICA_AUX_HPP
