//
// Point_light.hpp
//
// Description:
//  Point light implementation
//
//
// Authors:
//  Samuel García
//  Laura González
//
// Date:
//  11/2022.
//

#ifndef INFORMATICA_GRAFICA_POINTLIGHT_HPP
#define INFORMATICA_GRAFICA_POINTLIGHT_HPP

#include "math/Vector3d.hpp"
#include "../math/Point.hpp"


class PointLight {

public:

    Point center;
    Vector3d power;

    PointLight(Point _center, Vector3d _power): center(_center), power(_power) {}
};

#endif //INFORMATICA_GRAFICA_POINTLIGHT_HPP
