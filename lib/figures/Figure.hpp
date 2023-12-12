//
// Figure.hpp
//
// Description:
//  Figure interface
//
//
// Authors:
//  Samuel García
//  Laura González
//
// Date:
//  10/2022.
//

#ifndef INFORMATICA_GRAFICA_FIGURE_HPP
#define INFORMATICA_GRAFICA_FIGURE_HPP

#include <utility>

#include "../Ray.hpp"
#include "Bounds3d.hpp"
#include "HitRegister.hpp"
#include "Texture.hpp"

class Figure {
public:
    Vector3d diffuse_coefficient, refraction_coefficient, reflection_coefficient, emission;
    double refraction_index{};
    std::shared_ptr<Texture> texture;
    bool has_texture = false, is_area_light = false;

    Figure() = default;

    Figure(Vector3d _diffuse_coefficient,
           Vector3d _refraction_coefficient,
           Vector3d _reflection_coefficient,
           double _refraction_index) :
                diffuse_coefficient(_diffuse_coefficient),
                refraction_coefficient(_refraction_coefficient),
                reflection_coefficient(_reflection_coefficient),
                refraction_index(_refraction_index),
                texture(),
                has_texture(false),
                is_area_light(false) {}

    explicit Figure(Vector3d _emission) :
        emission(_emission),
        texture(),
        has_texture(false),
        is_area_light(true) {}

    Figure(Vector3d _diffuse_coefficient,
           Vector3d _refraction_coefficient,
           Vector3d _reflection_coefficient,
           double _refraction_index,
           std::shared_ptr<Texture> _texture) :
               diffuse_coefficient(_diffuse_coefficient),
               refraction_coefficient(_refraction_coefficient),
               reflection_coefficient(_reflection_coefficient),
               refraction_index(_refraction_index),
               texture(_texture),
               has_texture(true),
               is_area_light(false) {}

    [[nodiscard]] virtual HitRegister collides(const Ray &ray) const = 0;

    virtual Bounds3d bounds() const = 0;
    virtual ~Figure(){}

protected:
    // Only for textured figures
    virtual Vector3d get_texel_at_hit_point(HitRegister &reg) const {
        return reg.diffuse_coefficient;
    }

    // Quadratic solver adapted from:
    //  https://www.scratchapixel.com/lessons/3d-basic-rendering/minimal-ray-tracer-rendering-simple-shapes/ray-sphere-intersection
    static bool solveQuadratic(const double &a, const double &b, const double &c, double &x0, double &x1){
        double discr = b * b - 4 * a * c;
        if (less_than(discr, 0)) return false; // Se sabe que no hay solución si el discriminante es < 0
        else if (equals(discr, 0)) x0 = x1 = - 0.5 * b / a;
        else {
            double q = (greater_than(b, 0)) ?
                       -0.5 * (b + sqrt(discr)) :
                       -0.5 * (b - sqrt(discr));
            x0 = q / a;
            x1 = c / q;
        }

        if (greater_than(x0, x1)) std::swap(x0, x1);

        return true;
    }
};


#endif //INFORMATICA_GRAFICA_FIGURE_HPP
