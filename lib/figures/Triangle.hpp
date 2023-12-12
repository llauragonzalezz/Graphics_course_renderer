//
// Triangle.hpp
//
// Description:
//  Triangle figure implementation
//
//
// Authors:
//  Samuel García
//  Laura González
//
// Date:
//  10/2022.
//

#ifndef INFORMATICA_GRAFICA_TRIANGLE_HPP
#define INFORMATICA_GRAFICA_TRIANGLE_HPP
#include <algorithm>
#include "Figure.hpp"
#include "Plane.hpp"
#include "benchmarking.hpp"
#include "../math/TransformationMatrix.hpp"

class Triangle : public Figure {

public:
    Point a, b, c, vta, vtb, vtc;


    Triangle(Point _a, Point _b, Point _c,
             Vector3d _diffuse_coefficient,
             Vector3d _refraction_coefficient,
             Vector3d _reflection_coefficient,
             double _refraction_index) :
             Figure(_diffuse_coefficient,
                    _refraction_coefficient,
                    _reflection_coefficient,
                    _refraction_index), a(_a), b(_b), c(_c) {}

    Triangle(Point _a, Point _b, Point _c,
             Vector3d _diffuse_coefficient,
             Vector3d _refraction_coefficient,
             Vector3d _reflection_coefficient,
             double _refraction_index,
             std::shared_ptr<Texture> _texture,
             Point _vta, Point _vtb, Point _vtc) :
             Figure(_diffuse_coefficient,
                   _refraction_coefficient,
                   _reflection_coefficient,
                   _refraction_index,
                    std::move(_texture)),
             a(_a), b(_b), c(_c),
             vta(_vta), vtb(_vtb), vtc(_vtc) {}



    // Adapted from Möller–Trumbore's algorithm:
    //  https://en.wikipedia.org/wiki/M%C3%B6ller%E2%80%93Trumbore_intersection_algorithm
    HitRegister collides(const Ray &ray) const override {
        HitRegister reg;

#ifdef benchmarking
        Benchmarking::count_figure_checked();
#endif
        const double EPSILON = 0.0000001;
        Vector3d vertex0 = this->a.v;
        Vector3d vertex1 = this->b.v;
        Vector3d vertex2 = this->c.v;
        Vector3d edge1, edge2, h, s, q;
        double a, f, u, v;

        edge1 = vertex1 - vertex0;
        edge2 = vertex2 - vertex0;
        h = ray.direction.v * edge2;
        a = edge1.dot(h);
        if (a > -EPSILON && a < EPSILON) return reg; // r is parallel to the triangle

        f = 1.0/a;
        s = ray.origin.v - vertex0;
        u = f * s.dot(h);
        if (u < 0.0 || u > 1.0) return reg;

        q = s * edge1;
        v = f * ray.direction.v.dot(q);
        if (v < 0.0 || u + v > 1.0) return reg;

        double t = f * edge2.dot(q);
        if (t > EPSILON) { // Hit!
            reg.hits = true;
            reg.t = t;
            Direction normal = Direction((edge2 * edge1).normalize());
            reg.n = (less_than(ray.direction.v.dot(edge2 * edge1),0))?
                        Ray(Point(ray.origin.v + t*ray.direction.v), normal.v) :
                        Ray(Point(ray.origin.v + t*ray.direction.v), (-1)*normal.v);

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
        } else return reg;
    }

    Bounds3d bounds() const override {
        double xmin, ymin, zmin, xmax, ymax, zmax;

        xmin = std::min(std::min(a.v[0],b.v[0]), c.v[0]);
        ymin = std::min(std::min(a.v[1],b.v[1]), c.v[1]);
        zmin = std::min(std::min(a.v[2],b.v[2]), c.v[2]);
        xmax = std::max(std::max(a.v[0],b.v[0]), c.v[0]);
        ymax = std::max(std::max(a.v[1],b.v[1]), c.v[1]);
        zmax = std::max(std::max(a.v[2],b.v[2]), c.v[2]);

        return Bounds3d(Point(xmin,ymin,zmin), Point(xmax,ymax,zmax));
    }
private:
    Vector3d get_texel_at_hit_point(HitRegister &reg) const override {
        Vector3d p = reg.n.origin.v;

        // Adapted de https://computergraphics.stackexchange.com/questions/1866/how-to-map-square-texture-to-triangle,
        // which in turn is adapted from https://en.wikipedia.org/wiki/Barycentric_coordinate_system
        double bary_a, bary_b, bary_c;
        bary_a = (b[1] - c[1])*(p[0]-c[0]) + (c[0]-b[0])*(p[1]-c[1]);
        bary_a /= ((b[1] - c[1])*(this->a[0]-c[0]) + (c[0]-b[0])*(this->a[1]-c[1]));

        bary_b = (c[1] - this->a[1])*(p[0]-c[0]) + (this->a[0]-c[0])*(p[1]-c[1]);
        bary_b /= (b[1] - c[1])*(this->a[0]-c[0]) + (c[0]-b[0])*(this->a[1]-c[1]);

        bary_c = 1 - bary_a - bary_b;

        Vector3d P = bary_a * vta.v + bary_b * vtb.v + bary_c * vtc.v;


       return texture->get_texel(P[0],  1-P[1]);
    }
};
#endif //INFORMATICA_GRAFICA_TRIANGLE_HPP
