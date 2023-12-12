//
// Figure.hpp
//
// Description:
//
//  Plane implementation following the equation:
//      (p * n) + d = 0
//
// Authors:
//  Samuel García
//  Laura González
//
// Date:
//  10/2022.
//

#ifndef INFORMATICA_GRAFICA_PLANE_HPP
#define INFORMATICA_GRAFICA_PLANE_HPP

#include <utility>

#include "Figure.hpp"
#include "benchmarking.hpp"

class Plane : public Figure {

public:
    double d;
    Direction n; // normal

    // Solo para planos finitos
    bool bounded{false};
    Point min_bound{0,0,0};
    Point max_bound{0,0,0};

    Plane(double _d, Direction _n,
          Vector3d _diffuse_coefficient,
          Vector3d _refraction_coefficient,
          Vector3d _reflection_coefficient,
          double _refraction_index) :
                Figure(_diffuse_coefficient,
                     _refraction_coefficient,
                     _reflection_coefficient,
                     _refraction_index),
                d(_d), n(_n) {}

    Plane(double _d, Direction _n, Vector3d _emission) :
                Figure(_emission),
                d(_d), n(_n) {}

    Plane(double _d, Direction _n,
          Vector3d _diffuse_coefficient,
          Vector3d _refraction_coefficient,
          Vector3d _reflection_coefficient,
          double _refraction_index,
          std::shared_ptr<Texture> _texture) :

                Figure(_diffuse_coefficient,
                       _refraction_coefficient,
                       _reflection_coefficient,
                       _refraction_index,
                       std::move(_texture)),
                d(_d), n(_n) {}

    Plane(Point a, Point b, Point c, Vector3d _diffuse_coefficient) : Figure(_diffuse_coefficient) {
         n = Direction(((b.v - a.v)*(c.v - a.v)) / ((b.v - a.v)*(c.v - a.v)).modulus());
         d = a.v.dot(n.v);
    }

    // Fools the BVH by converting the plane to a finite rectangle (useful for area lights)
    void set_bounds(Point _min_bound, Point _max_bound) {
        min_bound = _min_bound;
        max_bound = _max_bound;
        bounded = true;
    }

    HitRegister collides(const Ray &ray) const override {
        HitRegister reg;

#ifdef benchmarking
        Benchmarking::count_figure_checked();
#endif

        auto t = - (d + (ray.origin.v).dot(n.v)) / (ray.direction.v).dot(n.v);

        auto p = ray.origin.v + t * ray.direction.v;

        reg.hits = greater_than(t, 0) && (equals((p.dot(n.v) + d) ,0));
        reg.t = t;
        reg.n = Ray(Point(p), Direction(n.v));

        if (has_texture) {
            reg.diffuse_coefficient = get_texel_at_hit_point(reg);
        } else {
            reg.diffuse_coefficient = diffuse_coefficient;
        }

        reg.emission = emission;
        reg.is_area_light = is_area_light;
        reg.refraction_coefficient = refraction_coefficient;
        reg.reflection_coefficient = reflection_coefficient;
        reg.refraction_index = refraction_index;

        return reg;
    }

    Bounds3d bounds() const override {
        if (bounded) {
            return {min_bound, max_bound};
        } else {
            double min = std::numeric_limits<double>::lowest();
            double max = std::numeric_limits<double>::max();

            return {Point(min, min, min), Point(max, max, max)};
        }
    }

private:

    Vector3d get_texel_at_hit_point(HitRegister &reg) const override {
        Vector3d u = computePrimaryTexDir(reg.n.direction.v);
        Vector3d v = n.v * u;

        double x = u.dot(reg.n.origin.v);
        double y = v.dot(reg.n.origin.v);

        // (Note: Requires the texture to be seamless
        if (x < 0) x = texture->texture_image.width + x;
        if (y < 0) y = texture->texture_image.height + y;
        return texture->get_texel(x, y);
    }

    // Adapted from:
    //     https://computergraphics.stackexchange.com/questions/8382/how-do-i-convert-a-hit-on-an-infinite-plane-to-uv-coordinates-for-texturing-in-a
    static Vector3d computePrimaryTexDir(Vector3d normal)
    {
        Vector3d a = normal * Vector3d(1, 0, 0);
        Vector3d b = normal * Vector3d(0, 1, 0);

        Vector3d max_ab = a.dot(a) < b.dot(b) ? b : a;

        Vector3d c = normal * Vector3d(0, 0, 1);

        return (max_ab.dot(max_ab) < c.dot(c) ? c : max_ab).normalize();
    }
};

#endif //INFORMATICA_GRAFICA_PLANE_HPP
