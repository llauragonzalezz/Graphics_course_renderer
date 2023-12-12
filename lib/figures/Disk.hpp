//
// Disk.hpp
//
// Description:
//  Disk figure implemented via ConstructiveSolidIntersection
//
// Authors:
//  Samuel García
//  Laura González
//
// Date:
//  10/2022.
//

#ifndef INFORMATICA_GRAFICA_DISK_HPP
#define INFORMATICA_GRAFICA_DISK_HPP
#include "Figure.hpp"
#include "Sphere.hpp"
#include "ConstructiveSolidIntersection.hpp"

class Disk : public Figure {
private:
    ConstructiveSolidIntersection figure;

public:
    Disk(Point _c, double _r,
         Vector3d _diffuse_coefficient,
         Vector3d _refraction_coefficient,
         Vector3d _reflection_coefficient,
         double _refraction_index) :
                Figure(_diffuse_coefficient,
                       _refraction_coefficient,
                       _reflection_coefficient,
                       _refraction_index) {
        //
        Sphere circ(_c, _r,
             _diffuse_coefficient,
             _refraction_coefficient,
             _reflection_coefficient,
             _refraction_index);

        Plane plane(_c[2], Direction(0, 0, -1),
                    Vector3d(0.6, 0.6, 0.6),
                    Vector3d(0, 0, 0),
                    Vector3d(0, 0, 0), 1.0);

        figure = ConstructiveSolidIntersection(std::make_shared<Plane>(plane), std::make_shared<Sphere>(circ));
    }

    [[nodiscard]] HitRegister collides(const Ray &ray) const override {
        HitRegister reg = figure.collides(ray);
        reg.diffuse_coefficient = diffuse_coefficient;
        reg.emission = emission;
        reg.is_area_light = is_area_light;
        reg.refraction_coefficient = refraction_coefficient;
        reg.reflection_coefficient = reflection_coefficient;
        reg.refraction_index = refraction_index;
        return reg;
    }

    [[nodiscard]] Bounds3d bounds() const override {
        return figure.bounds();
    }
};
#endif //INFORMATICA_GRAFICA_DISK_HPP
