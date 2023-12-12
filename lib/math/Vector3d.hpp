//
// Vector3d.hpp
//
// Description:
//
//  3D Vector implementation
//
// Authors:
//  Samuel García
//  Laura González
//
// Date:
//  10/2022.
//

#ifndef INFORMATICA_GRAFICA_VECTOR3D_HPP
#define INFORMATICA_GRAFICA_VECTOR3D_HPP

#include <math.h>
#include <iostream>

class Vector3d {
public:
    double c[3]{};

    Vector3d() {
        c[0] = 0.0;
        c[1] = 0.0;
        c[2] = 0.0;
    }

    Vector3d(double x, double y, double z) {
        c[0] = x;
        c[1] = y;
        c[2] = z;
    }

    // Vector op Vector

    Vector3d operator +(const Vector3d v) const {
        return {c[0] + v.c[0], c[1] + v.c[1], c[2] + v.c[2]};
    }

    Vector3d operator -(const Vector3d v) const {
        return {c[0] - v.c[0], c[1] - v.c[1], c[2] - v.c[2]};
    }

    Vector3d operator *(const Vector3d v) const {
        auto x = c[1] * v.c[2] - c[2] * v.c[1];
        auto y = c[2] * v.c[0] - c[0] * v.c[2];
        auto z = c[0] * v.c[1] - c[1] * v.c[0];

        return {x, y, z};
    }

    Vector3d element_by_element(const Vector3d v) const {
        auto x = c[0] * v.c[0];
        auto y = c[1] * v.c[1];
        auto z = c[2] * v.c[2];

        return {x, y, z};
    }

    double dot(const Vector3d v) const {
        return c[0] * v.c[0] + c[1] * v.c[1] + c[2] * v.c[2];
    }


    // Vector op double

    Vector3d operator *(const double s) const {
        return {c[0] * s , c[1] * s, c[2] * s};
    }

    friend Vector3d operator *(const double s, const Vector3d v) {
        return {v.c[0] * s , v.c[1] * s, v.c[2] * s};
    }

    Vector3d operator /(const double s) const {
        return (*this * (1/s));
    }

    friend Vector3d operator /(const double s, const Vector3d v) {
        return (v * (1/s));
    }

    // Misc. operators
    double operator[](int i) const {
        return c[i];
    }

    double& operator[](int i) {
        return c[i];
    }

    // Misc. methods

    //modulus <-> length
    double modulus() const {
        return sqrt(c[0]*c[0] + c[1]*c[1]  + c[2]*c[2]);
    }

    //normalization <-> unit
    Vector3d normalize() {
        return (*this / this->modulus());
    }

    friend std::ostream& operator<< (std::ostream& stream, const Vector3d& v) {
        stream << "(" << v.c[0] << " , " << v.c[1] << " , " << v.c[2] << ")";

        return stream;
    }
};

#endif //INFORMATICA_GRAFICA_VECTOR3D_HPP
