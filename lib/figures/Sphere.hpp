//
// Sphere.hpp
//
// Description:
//  Sphere figure implementation
//
//
// Authors:
//  Samuel García
//  Laura González
//
// Date:
//  10/2022.
//

#ifndef INFORMATICA_GRAFICA_SPHERE_HPP
#define INFORMATICA_GRAFICA_SPHERE_HPP

#include <utility>

#include "Figure.hpp"
#include "benchmarking.hpp"

class Sphere : public Figure {
private:
    Point center;
    double r;

public:
    Sphere(Point _center, double _r,
           Vector3d _diffuse_coefficient,
           Vector3d _refraction_coefficient,
           Vector3d _reflection_coefficient,
           double _refraction_index) :
                Figure(_diffuse_coefficient,
                       _refraction_coefficient,
                       _reflection_coefficient,
                       _refraction_index),
                center(_center), r(_r) {}

    Sphere(Point _center, double _r, Vector3d _emission) :
                Figure(_emission),

                center(_center), r(_r) {}

    Sphere(Point _center, double _r,
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

                center(_center), r(_r) {}

    HitRegister collides(const Ray &ray) const override {
        HitRegister reg;

#ifdef benchmarking
        Benchmarking::count_figure_checked();
#endif

        double raiz1 = 0, raiz2 = 0;

        double a = ray.direction.v.modulus()*ray.direction.v.modulus();
        double b = 2*ray.direction.v.dot(ray.origin.v - center.v);
        double c = (ray.origin.v - center.v).modulus()*(ray.origin.v - center.v).modulus() - r*r;

        auto existeRaiz = solveQuadratic(a, b, c, raiz1, raiz2);

        double t = raiz1, t_max = raiz2;

        if (less_than(t, 0)) {
            t = raiz2;
            t_max = std::numeric_limits<double>::min();;
            if (less_than(t, 0)) return reg;
        }

        reg.hits = existeRaiz && (greater_than(raiz1, 0) || greater_than(raiz2, 0));
        reg.t = t;
        if (greater_or_equal(raiz1, 0)) {
            reg.t = raiz1;
        } else {
            reg.t = raiz2;
        }
        reg.t_max = t_max;

        auto p = ray.origin.v + t*ray.direction.v;
        reg.n = Ray(Point(p), Direction((p - this->center.v)));

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
        Point min = Point(Vector3d(-r, -r, -r) + center.v);
        Point max = Point(Vector3d(r, r, r) + center.v);

        return {min, max};
    }

private:

    Vector3d get_texel_at_hit_point(HitRegister &reg) const override {
        // Adapted for non-unit spheres from:
        //  http://www.bentonian.com/Lectures/AdvGraph1314/3.%20Ray%20tracing%20-%20color%20and%20texture.pdf
        double u = r + (atan2(reg.n.direction[2], reg.n.direction[0]) / (2 * M_PI));
        double v = r - (asin(reg.n.direction[1]) / M_PI);

        // In PPM's, the upper left corner is (0,0), so we turn u and v around
        u = 1 - std::abs(u);
        v = 1 - std::abs(v);

        return texture->get_texel(u, v);
    }
};

#endif //INFORMATICA_GRAFICA_SPHERE_HPP
