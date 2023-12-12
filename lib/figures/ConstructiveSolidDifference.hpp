//
// ConstructiveSolidDifference.hpp
//
// Description:
//  Figure subtraction wrapper
//
// Authors:
//  Samuel García
//  Laura González
//
// Date:
//  11/2022.
//

#ifndef INFORMATICA_GRAFICA_CONSTRUCTIVESOLIDDIFFERENCE_HPP
#define INFORMATICA_GRAFICA_CONSTRUCTIVESOLIDDIFFERENCE_HPP
#include <vector>
#include <memory>
#include "Figure.hpp"
#include "../math/TransformationMatrix.hpp"

class ConstructiveSolidDifference : public Figure {
public:
    std::shared_ptr<Figure> fig1;
    std::shared_ptr<Figure> fig2;


    ConstructiveSolidDifference(std::shared_ptr<Figure> _fig1, std::shared_ptr<Figure> _fig2) {
            fig1 = _fig1;
            fig2 = _fig2;
    }

    HitRegister collides(const Ray &ray) const override {
        HitRegister reg1 = fig1->collides(ray);
        HitRegister reg2 = fig2->collides(ray);

        if (reg1.hits && reg2.hits) {
            if (reg1.t < reg2.t) return reg1;
            else if (reg2.t_max < reg1.t_max) {
                reg2.t = reg2.t_max;
                return reg2;
            }

        } else if (reg1.hits) return reg1;

        return HitRegister();
    }

    Bounds3d bounds() const override {
        Bounds3d box1 = fig1->bounds();
        Bounds3d box2 = fig2->bounds();
        float xmin, ymin, zmin, xmax, ymax, zmax;

        xmin = std::min(box1.p_min.v[0], box2.p_min.v[0]);
        ymin = std::min(box1.p_min.v[1], box2.p_min.v[1]);
        zmin = std::min(box1.p_min.v[2], box2.p_min.v[2]);
        xmax = std::max(box1.p_max.v[0], box2.p_max.v[0]);
        ymax = std::max(box1.p_max.v[1], box2.p_max.v[1]);
        zmax = std::max(box1.p_max.v[2], box2.p_max.v[2]);

        return Bounds3d(Point(xmin, ymin, zmin), Point(xmax, ymax, zmax));
    }
};


#endif //INFORMATICA_GRAFICA_CONSTRUCTIVESOLIDDIFFERENCE_HPP
