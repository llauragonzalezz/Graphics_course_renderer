//
// TransformationMatrix.hpp
//
// Description:
//  Transformation matrix implementation, using homogeneous coordinates
//
// Authors:
//  Samuel García
//  Laura González
//
// Date:
//  10/2022.
//

#ifndef INFORMATICA_GRAFICA_TRANSFORMATIONMATRIX_HPP
#define INFORMATICA_GRAFICA_TRANSFORMATIONMATRIX_HPP

#include <cmath>
#include "Vector3d.hpp"
#include "Direction.hpp"

class TransformationMatrix {
private:
    double m[4][4]{};

public:
    explicit TransformationMatrix(double t00, double t01, double t02, double t03,
                                  double t10, double t11, double t12, double t13,
                                  double t20, double t21, double t22, double t23,
                                  double t30, double t31, double t32, double t33) {
        m[0][0] = t00; m[1][0] = t10; m[2][0] = t20; m[3][0] = t30;
        m[0][1] = t01; m[1][1] = t11; m[2][1] = t21; m[3][1] = t31;
        m[0][2] = t02; m[1][2] = t12; m[2][2] = t22; m[3][2] = t32;
        m[0][3] = t03; m[1][3] = t13; m[2][3] = t23; m[3][3] = t33;
    }

    explicit TransformationMatrix() {
        m[0][0] = 0; m[1][0] = 0; m[2][0] = 0; m[3][0] = 0;
        m[0][1] = 0; m[1][1] = 0; m[2][1] = 0; m[3][1] = 0;
        m[0][2] = 0; m[1][2] = 0; m[2][2] = 0; m[3][2] = 0;
        m[0][3] = 0; m[1][3] = 0; m[2][3] = 0; m[3][3] = 0;
    }

    static TransformationMatrix TranslationMatrix(double tx, double ty, double tz) {
        return TransformationMatrix(1.0, 0.0, 0.0, tx,
                                    0.0, 1.0, 0.0, ty,
                                    0.0, 0.0, 1.0, tz,
                                    0.0, 0.0, 0.0, 1.0);
    }

    static TransformationMatrix ScaleMatrix(double sx, double sy, double sz) {
        return TransformationMatrix(sx, 0.0, 0.0, 0.0,
                                    0.0, sy, 0.0, 0.0,
                                    0.0, 0.0, sz, 0.0,
                                    0.0, 0.0, 0.0, 1.0);
    }

    static TransformationMatrix RotationMatrixOnX(double rads) {
        return TransformationMatrix(1.0, 0.0, 0.0, 0.0,
                                    0.0, cos(rads), -sin(rads), 0.0,
                                    0.0, sin(rads), cos(rads), 0.0,
                                    0.0, 0.0, 0.0, 1.0);
    }

    static TransformationMatrix RotationMatrixOnY(double rads) {
        return TransformationMatrix(cos(rads), 0.0, sin(rads), 0.0,
                                    0.0, 1.0, 0.0, 0.0,
                                    -sin(rads), 0.0, cos(rads), 0.0,
                                    0.0, 0.0, 0.0, 1.0);
    }

    static TransformationMatrix RotationMatrixOnZ(double rads) {
        return TransformationMatrix(cos(rads), -sin(rads), 0.0, 0.0,
                                    sin(rads), cos(rads), 0.0, 0.0,
                                    0.0, 0.0, 1.0, 0.0,
                                    0.0, 0.0, 0.0, 1.0);
    }

    static TransformationMatrix BaseChangeMatrix(Vector3d u, Vector3d v, Vector3d w, Vector3d o) {
        return TransformationMatrix(u.c[0], v.c[0], w.c[0], o.c[0],
                                    u.c[1], v.c[1], w.c[1], o.c[1],
                                    u.c[2], v.c[2], w.c[2], o.c[2],
                                    0.0, 0.0, 0.0, 1.0);
    }

    TransformationMatrix transpose() const{
        return TransformationMatrix(m[0][0], m[1][0], m[2][0], m[3][0],
                                    m[0][1], m[1][1], m[2][1], m[3][1],
                                    m[0][2], m[1][2], m[2][2], m[3][2],
                                    m[0][3], m[1][3], m[2][3], m[3][3]);
    }

    TransformationMatrix operator *(TransformationMatrix m2) {
        TransformationMatrix res;

        for (int i = 0; i < 4; ++i)
            for (int j = 0; j < 4; ++j)
                res.m[i][j] = m[i][0] * m2.m[0][j] +
                              m[i][1] * m2.m[1][j] +
                              m[i][2] * m2.m[2][j] +
                              m[i][3] * m2.m[3][j];
        return res;
    }

    Vector3d operator *(Vector3d x) const {
        Vector3d v;

        for (int i = 0; i < 3; ++i)
            v.c[i] = m[i][0] * x[0] +
                     m[i][1] * x[1] +
                     m[i][2] * x[2];
        return v;
    }

    TransformationMatrix operator *(double f) {
        return TransformationMatrix(m[0][0]*f, m[0][1]*f, m[0][2]*f, m[0][3]*f,
                                    m[1][0]*f, m[1][1]*f, m[1][2]*f, m[1][3]*f,
                                    m[2][0]*f, m[2][1]*f, m[2][2]*f, m[2][3]*f,
                                    m[3][0]*f, m[3][1]*f, m[3][2]*f, m[3][3]*f);
    }

    TransformationMatrix operator /(double f) {
        return *this * (1.0f/f);
    }

    // Adapted from MESA's (?) implementation:
    //      https://stackoverflow.com/questions/1148309/inverting-a-4x4-matrix
    //      https://www.mathsisfun.com/algebra/matrix-inverse-minors-cofactors-adjugate.html
    TransformationMatrix inverse() const{
        TransformationMatrix inv;
        double det;

        inv.m[0][0] = m[1][1] * m[2][2]* m[3][3]-
                    m[1][1] * m[2][3]* m[3][2]-
                    m[2][1] * m[1][2] * m[3][3]+
                    m[2][1] * m[1][3] * m[3][2]+
                    m[3][1]* m[1][2] * m[2][3]-
                    m[3][1]* m[1][3] * m[2][2];

        inv.m[1][0] = -m[1][0] * m[2][2]* m[3][3]+
                    m[1][0] * m[2][3]* m[3][2]+
                    m[2][0] * m[1][2] * m[3][3]-
                    m[2][0] * m[1][3] * m[3][2]-
                    m[3][0]* m[1][2] * m[2][3]+
                    m[3][0]* m[1][3] * m[2][2];

        inv.m[2][0] = m[1][0] * m[2][1]* m[3][3]-
                    m[1][0] * m[2][3]* m[3][1]-
                    m[2][0] * m[1][1]* m[3][3]+
                    m[2][0] * m[1][3]* m[3][1]+
                    m[3][0]* m[1][1]* m[2][3]-
                    m[3][0]* m[1][3]* m[2][1];

        inv.m[3][0] = -m[1][0] * m[2][1]* m[3][2]+
                    m[1][0] * m[2][2]* m[3][1]+
                    m[2][0] * m[1][1]* m[3][2]-
                    m[2][0] * m[1][2]* m[3][1]-
                    m[3][0]* m[1][1]* m[2][2]+
                    m[3][0]* m[1][2]* m[2][1];

        inv.m[0][1] = -m[0][1] * m[2][2]* m[3][3]+
                    m[0][1] * m[2][3]* m[3][2]+
                    m[2][1] * m[0][2]* m[3][3]-
                    m[2][1] * m[0][3]* m[3][2]-
                    m[3][1]* m[0][2]* m[2][3]+
                    m[3][1]* m[0][3]* m[2][2];

        inv.m[1][1] = m[0][0] * m[2][2]* m[3][3]-
                    m[0][0] * m[2][3]* m[3][2]-
                    m[2][0] * m[0][2]* m[3][3]+
                    m[2][0] * m[0][3]* m[3][2]+
                    m[3][0]* m[0][2]* m[2][3]-
                    m[3][0]* m[0][3]* m[2][2];

        inv.m[2][1] = -m[0][0] * m[2][1]* m[3][3]+
                    m[0][0] * m[2][3]* m[3][1]+
                    m[2][0] * m[0][1]* m[3][3]-
                    m[2][0] * m[0][3]* m[3][1]-
                    m[3][0]* m[0][1]* m[2][3]+
                    m[3][0]* m[0][3]* m[2][1];

        inv.m[3][1] = m[0][0] * m[2][1]* m[3][2]-
                    m[0][0] * m[2][2]* m[3][1]-
                    m[2][0] * m[0][1]* m[3][2]+
                    m[2][0] * m[0][2]* m[3][1]+
                    m[3][0]* m[0][1]* m[2][2]-
                    m[3][0]* m[0][2]* m[2][1];

        inv.m[0][2] = m[0][1] * m[1][2]* m[3][3]-
                    m[0][1] * m[1][3]* m[3][2]-
                    m[1][1] * m[0][2]* m[3][3]+
                    m[1][1] * m[0][3]* m[3][2]+
                    m[3][1]* m[0][2]* m[1][3]-
                    m[3][1]* m[0][3]* m[1][2];

        inv.m[1][2] = -m[0][0] * m[1][2]* m[3][3]+
                    m[0][0] * m[1][3]* m[3][2]+
                    m[1][0] * m[0][2]* m[3][3]-
                    m[1][0] * m[0][3]* m[3][2]-
                    m[3][0]* m[0][2]* m[1][3]+
                    m[3][0]* m[0][3]* m[1][2];

        inv.m[2][2] = m[0][0] * m[1][1]* m[3][3]-
                    m[0][0] * m[1][3]* m[3][1]-
                    m[1][0] * m[0][1]* m[3][3]+
                    m[1][0] * m[0][3]* m[3][1]+
                    m[3][0]* m[0][1]* m[1][3]-
                    m[3][0]* m[0][3]* m[1][1];

        inv.m[3][2] = -m[0][0] * m[1][1]* m[3][2]+
                    m[0][0] * m[1][2]* m[3][1]+
                    m[1][0] * m[0][1]* m[3][2]-
                    m[1][0] * m[0][2]* m[3][1]-
                    m[3][0]* m[0][1]* m[1][2]+
                    m[3][0]* m[0][2]* m[1][1];

        inv.m[0][3] = -m[0][1]* m[1][2]* m[2][3]+
                    m[0][1]* m[1][3]* m[2][2]+
                    m[1][1]* m[0][2]* m[2][3]-
                    m[1][1]* m[0][3]* m[2][2]-
                    m[2][1]* m[0][2]* m[1][3]+
                    m[2][1]* m[0][3]* m[1][2];

        inv.m[1][3] = m[0][0]* m[1][2]* m[2][3]-
                    m[0][0]* m[1][3]* m[2][2]-
                    m[1][0]* m[0][2]* m[2][3]+
                    m[1][0]* m[0][3]* m[2][2]+
                    m[2][0]* m[0][2]* m[1][3]-
                    m[2][0]* m[0][3]* m[1][2];

        inv.m[2][3] = -m[0][0]* m[1][1]* m[2][3]+
                    m[0][0]* m[1][3]* m[2][1]+
                    m[1][0]* m[0][1]* m[2][3]-
                    m[1][0]* m[0][3]* m[2][1]-
                    m[2][0]* m[0][1]* m[1][3]+
                    m[2][0]* m[0][3]* m[1][1];

        inv.m[3][3] = m[0][0]* m[1][1]* m[2][2]-
                    m[0][0]* m[1][2]* m[2][1]-
                    m[1][0]* m[0][1]* m[2][2]+
                    m[1][0]* m[0][2]* m[2][1]+
                    m[2][0]* m[0][1]* m[1][2]-
                    m[2][0]* m[0][2]* m[1][1];

        det = m[0][0]* inv.m[0][0]
                + m[0][1]* inv.m[1][0]
                + m[0][2]* inv.m[2][0]
                + m[0][3]* inv.m[3][0];

        if (det == 0)
            throw std::runtime_error("Attempted to calculate the inverse of a singular matrix!");

        inv = inv / det;

        return inv;
    }

    friend std::ostream& operator<< (std::ostream& stream, const TransformationMatrix& t) {
        stream << "/"   << t.m[0][0]    << " " <<  t.m[0][1]    << " " <<  t.m[0][2]    << " " << t.m[0][3] << "\\\n";
        stream << "|"   << t.m[1][0]    << " " << t.m[1][1]     << " " << t.m[1][2]     << " " << t.m[1][3] << "|\n";
        stream << "|"   << t.m[2][0]    << " " << t.m[2][1]     << " " << t.m[2][2]     << " " << t.m[2][3] << "|\n";
        stream << "\\"  << t.m[3][0]    << " " <<  t.m[3][1]    << " " <<  t.m[3][2]    << " " << t.m[3][3] << "/\n";

        return stream;
    }
};


#endif //INFORMATICA_GRAFICA_TRANSFORMATIONMATRIX_HPP
