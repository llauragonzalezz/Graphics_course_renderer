//
// Ray.hpp
//
// Description:
//  Ray implementation
//
//
// Authors:
//  Samuel García
//  Laura González
//
// Date:
//  10/2022.
//

#ifndef INFORMATICA_GRAFICA_RAY_HPP
#define INFORMATICA_GRAFICA_RAY_HPP

#include "math/Point.hpp"

class Ray {
public:
    Point origin;
    Direction direction;

    Vector3d flux{0,0,0}; // Only used during Photonmapping, in order to know the original flux from the point light

    Ray() = default;
    Ray(Point p, Direction d): origin(p), direction(Direction(d.v.normalize())) {}

    Ray(Point p, Direction d, Vector3d _flux): origin(p), direction(Direction(d.v.normalize())), flux(_flux) {}

};



#endif //INFORMATICA_GRAFICA_RAY_HPP

