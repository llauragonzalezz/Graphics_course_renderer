//
// Point.hpp
//
// Description:
//  Point implementation (only for a cleaner distinction from directions)
//
// Authors:
//  Samuel García
//  Laura González
//
// Date:
//  10/2022.
//

#ifndef INFORMATICA_GRAFICA_POINT_H
#define INFORMATICA_GRAFICA_POINT_H

#include "Vector3d.hpp"
#include "Direction.hpp"

class Point {
public:
    Vector3d v;

    Point() = default;

    Point(double x, double y, double z) : v(x, y, z) {}

    explicit Point(Vector3d _v) : v(_v) {}

    Point traslation(Direction d){
        return Point(v + d.v);
    }

    double operator[](int i) const {
        return v[i];
    }
};


#endif //INFORMATICA_GRAFICA_POINT_H
