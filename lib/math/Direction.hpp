//
// Direction.hpp
//
// Description:
//
//  Direction implementation (only for a cleaner distinction from points)
//
// Authors:
//  Samuel García
//  Laura González
//
// Date:
//  10/2022.
//

#ifndef INFORMATICA_GRAFICA_DIRECTION_HPP
#define INFORMATICA_GRAFICA_DIRECTION_HPP

#include "Vector3d.hpp"

class Direction  {
public:
    Vector3d v;

    Direction() = default;

    Direction(double x, double y, double z) : v(x, y, z) {}

    Direction(Vector3d v) : v(v) {}

    double operator[](int i) const {
        return v[i];
    }
};


#endif //INFORMATICA_GRAFICA_DIRECTION_HPP
