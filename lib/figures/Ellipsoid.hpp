//
// Disk.hpp
//
// Description:
//  Ellipsoid figure implemented via TransformedFigure
//
// Authors:
//  Samuel García
//  Laura González
//
// Date:
//  10/2022.
//

#ifndef INFORMATICA_GRAFICA_ELLIPSOID_HPP
#define INFORMATICA_GRAFICA_ELLIPSOID_HPP
#include "Figure.hpp"
#include "TransformedFigure.hpp"

class Ellipsoid : public Figure {
private:
    TransformedFigure ellipsoid;

public:

    Ellipsoid(Point _o, double _a, double _b, double _c,
              Vector3d _diffuse_coefficient,
              Vector3d _refraction_coefficient,
              Vector3d _reflection_coefficient,
              double _refraction_index) :
                    Figure(_diffuse_coefficient,
                           _refraction_coefficient,
                           _reflection_coefficient,
                           _refraction_index) {


        Sphere circ = Sphere(_o, 1,
                             _diffuse_coefficient,
                             _refraction_coefficient,
                             _reflection_coefficient,
                             _refraction_index);

        TransformationMatrix m = TransformationMatrix::ScaleMatrix(_a,_b,_c);
        ellipsoid = TransformedFigure(m, std::make_shared<Sphere>(circ),
                        _diffuse_coefficient,
                          _refraction_coefficient,
                          _reflection_coefficient,
                          _refraction_index);
    }

    [[nodiscard]] HitRegister collides(const Ray &ray) const override {
        return ellipsoid.collides(ray);
    }

    [[nodiscard]] Bounds3d bounds() const override {
        return ellipsoid.bounds();
    }
};
#endif //INFORMATICA_GRAFICA_ELLIPSOID_HPP
