//  
// Bounds3d.hpp
// 
// Description:
//  Bounding Box implementation, based on Physically Based Rendering: From Theory to Implementation's own Bounding Box
//
//  Bounding Boxes are aligned to global scene coordinates
//
// Authors:
//  Samuel García
//  Laura González
//
// Date:
//  10/2022.
//

#ifndef INFORMATICA_GRAFICA_BOUNDS3D_HPP
#define INFORMATICA_GRAFICA_BOUNDS3D_HPP

#include <limits>
#include <tuple>
#include "../math/Point.hpp"
#include "aux.hpp"
#include "HitRegister.hpp"
#include "../Ray.hpp"
#ifdef benchmarking
#include "benchmarking.hpp"
#endif

class Bounds3d {
public:
    Point p_min, p_max;

    Bounds3d() {
        double minNum = std::numeric_limits<double>::lowest();
        double maxNum = std::numeric_limits<double>::max();
        p_min = Point(maxNum, maxNum, maxNum);
        p_max = Point(minNum, minNum, minNum);
    }

    // Bounding Box enclosing two points, each one representing a corner
    [[nodiscard]] Bounds3d(const Point p1, const Point p2)
            : p_min(p1[0],
                    p1[1],
                    p1[2]),

              p_max(p2[0],
                    p2[1],
                    p2[2]) {}


    // Returns a Bounding box containing itself and the given point
    [[nodiscard]] Bounds3d Union(const Point &p) const {
        return {Point(std::min(p_min[0], p[0]),
                            std::min(p_min[1], p[1]),
                            std::min(p_min[2], p[2])),
                Point(std::max(p_max[0], p[0]),
                            std::max(p_max[1], p[1]),
                            std::max(p_max[2], p[2]))};
    }

    // Returns a Bounding box containing itself and the given Bound Box
    [[nodiscard]] Bounds3d Union(const Bounds3d &b) const {
        return {Point(std::min(p_min[0], b.p_min[0]),
                      std::min(p_min[1], b.p_min[1]),
                      std::min(p_min[2], b.p_min[2])),
                Point(std::max(p_max[0], b.p_max[0]),
                      std::max(p_max[1], b.p_max[1]),
                      std::max(p_max[2], b.p_max[2]))};
    }

    // Return's the biggest axis index
    // 0 (x), 1(y) or 2(z)
    [[nodiscard]] int maximum_extent() const {
        Vector3d  d = p_max.v - p_min.v; // Diagonal

        if (d[0] > d[1] && d[0] > d[2])
            return 0;
        else if (d[1] > d[2])
            return 1;
        else
            return 2;
    }

    // Optimized Bounding Box collision, adapted from:
    //  https://raytracing.github.io/books/RayTracingTheNextWeek.html#boundingvolumehierarchies
    [[nodiscard]] std::tuple<bool, double, double>  collides(const Ray &ray) const {
#ifdef benchmarking
        Benchmarking::count_bounds_checked();
#endif
        double t_min = std::numeric_limits<double>::min();
        double t_max = std::numeric_limits<double>::max();

        for (int a = 0; a < 3; a++) {
            auto invD = 1.0f / ray.direction[a];
            auto t0 = (p_min[a] - ray.origin[a]) * invD;
            auto t1 = (p_max[a] - ray.origin[a]) * invD;
            if (invD < 0.0f)
                std::swap(t0, t1);
            t_min = t0 > t_min ? t0 : t_min;
            t_max = t1 < t_max ? t1 : t_max;
            if (t_max <= t_min)
                return std::make_tuple(false, t_min, t_max);
        }
        return std::make_tuple(true, t_min, t_max);
    }
};

#endif //INFORMATICA_GRAFICA_BOUNDS3D_HPP
