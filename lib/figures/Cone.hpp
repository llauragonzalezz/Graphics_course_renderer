//
// Cone.hpp
//
// Description:
//  Cone figure
//
// Authors:
//  Samuel García
//  Laura González
//
// Date:
//  10/2022.
//

#ifndef INFORMATICA_GRAFICA_CONE_H
#define INFORMATICA_GRAFICA_CONE_H
#include "Figure.hpp"

class Cone : public Figure {
private:
    Point c;
    double h;

public:
    Cone(Point _c, double _h,
         Vector3d _diffuse_coefficient,
         Vector3d _refraction_coefficient,
         Vector3d _reflection_coefficient,
         double _refraction_index) :
            Figure(_diffuse_coefficient,
                   _refraction_coefficient,
                   _reflection_coefficient,
                   _refraction_index),
            c(_c), h(_h)  {
    }

    HitRegister collides(const Ray &ray) const override {
        HitRegister reg;

        double raiz1 = 0.0, raiz2 = 0.0;

        auto a =  ray.direction.v[0] * ray.direction.v[0] +
                         ray.direction.v[1] * ray.direction.v[1] -
                         ray.direction.v[2] * ray.direction.v[2];

        auto b = 2*(ray.direction.v[0] * ray.origin.v[0] - ray.direction.v[0] * this->c.v[0] +
                           ray.direction.v[1] * ray.origin.v[1] - ray.direction.v[1] * this->c.v[1] -
                          (ray.direction.v[2] * ray.origin.v[2] - ray.direction.v[2] * this->c.v[2]));

        auto c = ray.origin.v[0] * ray.origin.v[0] - 2*ray.origin.v[0] * this->c.v[0] + this->c.v[0] * this->c.v[0] +
                        ray.origin.v[1] * ray.origin.v[1] - 2*ray.origin.v[1] * this->c.v[1] + this->c.v[1] * this->c.v[1] -
                       (ray.origin.v[2] * ray.origin.v[2] - 2*ray.origin.v[2] * this->c.v[2] + this->c.v[2] * this->c.v[2]);

        auto existeRaiz = solveQuadratic(a, b, c, raiz1, raiz2);

        double t = raiz1;

        if (less_than(t, 0)) {
            t = raiz2;
            if (less_than(t, 0)) return reg;
        }

        // Check the intersection's height
        auto p = ray.origin.v + t*ray.direction.v;
        if (p.c[2] > this->c.v[2]  || p.c[2] < this->c.v[2] - h) return reg;

        reg.hits = existeRaiz && (greater_than(raiz1, 0)|| greater_than(raiz2, 0));
        reg.t = t;
        reg.n = Ray(Point(p), Direction(2*(p.c[0] - this->c.v[0]), 2*(p.c[1] - this->c.v[1]), 2*(p.c[2] - this->c.v[2])));
        reg.diffuse_coefficient = diffuse_coefficient;
        reg.emission = emission;
        reg.is_area_light = is_area_light;
        reg.refraction_coefficient = refraction_coefficient;
        reg.reflection_coefficient = reflection_coefficient;
        reg.refraction_index = refraction_index;

        return reg;
    }

    Bounds3d bounds() const override {
        return Bounds3d(Point(c.v[0] - sqrt(h), c.v[1] - sqrt(h),  c.v[2] - h),
                        Point(c.v[0] + sqrt(h), c.v[1] + sqrt(h),  c.v[2]));
    }
};
#endif //INFORMATICA_GRAFICA_CONE_H
