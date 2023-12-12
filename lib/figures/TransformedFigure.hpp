//
// TransformedFigure.hpp
//
// Description:
//  Wrapper for a figure to which we have applied transformation matrices
//
//
// Authors:
//  Samuel García
//  Laura González
//
// Date:
//  11/2022.
//

#ifndef INFORMATICA_GRAFICA_TRANSFORMEDFIGURE_HPP
#define INFORMATICA_GRAFICA_TRANSFORMEDFIGURE_HPP
#include <memory>
#include "Figure.hpp"
#include "../math/TransformationMatrix.hpp"

class TransformedFigure : public Figure {

public:
    TransformationMatrix m;
    std::shared_ptr<Figure> fig;

    TransformedFigure(){};
    TransformedFigure(TransformationMatrix _m,
                      std::shared_ptr<Figure> _fig,
                      Vector3d _diffuse_coefficient,
                      Vector3d _refraction_coefficient,
                      Vector3d _reflection_coefficient,
                      double _refraction_index) :
                            Figure(_diffuse_coefficient,
                                _refraction_coefficient,
                                _reflection_coefficient,
                                _refraction_index),
                            m(_m), fig(_fig){}


    HitRegister collides(const Ray &ray) const override {
        Ray rtransformado = Ray(Point(m.inverse()*ray.origin.v) , Direction(m.inverse()*ray.direction.v));
        HitRegister reg = fig->collides(rtransformado);

        if (reg.hits){
            reg.n.origin = Point(m*reg.n.origin.v);
            reg.n.direction = Direction(m.inverse().transpose()*reg.n.direction.v);
        }

        return reg;
    }

    Bounds3d bounds() const override {
        Bounds3d box = fig->bounds();
        Point pmin = Point(m*box.p_min.v);
        Point pmax = Point(m*box.p_max.v);
        double xmin, ymin, zmin, xmax, ymax, zmax;

        xmin = std::min(std::min(std::min(pmin.v[0],pmax.v[0]),box.p_min.v[0]),box.p_max.v[0]);
        ymin = std::min(std::min(std::min(pmin.v[1],pmax.v[1]),box.p_min.v[1]),box.p_max.v[1]);
        zmin = std::min(std::min(std::min(pmin.v[2],pmax.v[2]),box.p_min.v[2]),box.p_max.v[2]);
        xmax = std::max(std::max(std::max(pmin.v[0],pmax.v[0]),box.p_min.v[0]),box.p_max.v[0]);
        ymax = std::max(std::max(std::max(pmin.v[1],pmax.v[1]),box.p_min.v[1]),box.p_max.v[1]);
        zmax = std::max(std::max(std::max(pmin.v[2],pmax.v[2]),box.p_min.v[2]),box.p_max.v[2]);

        return Bounds3d(Point(xmin,ymin,zmin), Point(xmax,ymax,zmax));
    }
};



#endif //INFORMATICA_GRAFICA_TRANSFORMEDFIGURE_HPP
