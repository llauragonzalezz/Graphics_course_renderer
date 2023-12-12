//
// ConstructiveSolidDifference.hpp
//
// Description:
//  Cylinder figure
//
// Authors:
//  Samuel García
//  Laura González
//
// Date:
//  10/2022.
//

#ifndef INFORMATICA_GRAFICA_CYLINDER_HPP
#define INFORMATICA_GRAFICA_CYLINDER_HPP

#include "Figure.hpp"

class Cylinder : public Figure {
private:
    Point c;
    double r, h;

public:
    Cylinder(Point _c, double _r, double _h,
             Vector3d _diffuse_coefficient,
             Vector3d _refraction_coefficient,
             Vector3d _reflection_coefficient,
             double _refraction_index) :
                    Figure(_diffuse_coefficient,
                           _refraction_coefficient,
                           _reflection_coefficient,
                           _refraction_index),
                    c(_c), r(_r), h(_h) {}

    HitRegister collides(const Ray &ray) const override {
        HitRegister reg;

        double raiz1 = 0.0, raiz2 = 0.0;

        auto a = ray.direction.v[0] * ray.direction.v[0] + ray.direction.v[1] * ray.direction.v[1];

        auto b = 2*(ray.direction.v[0] * ray.origin.v[0] - ray.direction.v[0] * this->c.v[0] +
                           ray.direction.v[1] * ray.origin.v[1] - ray.direction.v[1] * this->c.v[1]);

        auto c = ray.origin.v[0] * ray.origin.v[0] - 2*ray.origin.v[0] * this->c.v[0] + this->c.v[0] * this->c.v[0] +
                        ray.origin.v[1] * ray.origin.v[1] - 2*ray.origin.v[1] * this->c.v[1] + this->c.v[1] * this->c.v[1] - r * r;


        auto existeRaiz = solveQuadratic(a, b, c, raiz1, raiz2);

        double t = raiz1, t_max = raiz2;

        if (less_than(t, 0)) {
            t = raiz2;
            t_max = std::numeric_limits<double>::min();
            if (less_than(t, 0)) return reg;
        }

        auto p = ray.origin.v + t*ray.direction.v;

        if (greater_than(p.c[2],this->c.v[2]) || less_than(p.c[2],this->c.v[2] - h)) return reg;

        reg.hits = existeRaiz && (greater_than(raiz1, 0)|| greater_than(raiz2, 0));
        reg.t = t;
        reg.t_max = t_max;
        reg.n = Ray(Point(p), Direction(2*(p.c[0] - this->c.v[0]), 2*(p.c[1] - this->c.v[1]), 0).v.normalize());
        reg.diffuse_coefficient = diffuse_coefficient;
        reg.emission = emission;
        reg.is_area_light = is_area_light;
        reg.refraction_coefficient = refraction_coefficient;
        reg.reflection_coefficient = reflection_coefficient;
        reg.refraction_index = refraction_index;

        return reg;
    }

    Bounds3d bounds() const override {
        return Bounds3d(Point(c.v[0] - r, c.v[1] - r, c.v[2] - h), Point(c.v[0] + r, c.v[1] + r, c.v[2] + h));
    }
};
#endif //INFORMATICA_GRAFICA_CYLINDER_H
