//
// photon.hpp
//
// Description:
//  Photon implementation
//
// Authors:
//  Samuel García
//  Laura González
//
// Date:
//  11/2022.
//

#ifndef INFORMATICA_GRAFICA_PHOTON_HPP
#define INFORMATICA_GRAFICA_PHOTON_HPP

#include "../math/Point.hpp"
#include "../Ray.hpp"

class Photon {
public:
        Point position;
        Direction indicent_direction;
        Vector3d flux;

        Photon(Point _position, Direction _indicent_direction, Vector3d _flux) : position(_position), indicent_direction(_indicent_direction), flux(_flux) {}

        double operator[](size_t i) const {
            return position[i];
        }

};

struct PhotonAxisPosition {
    double operator()(const Photon& p, size_t i) const {
        return p[i];
    }
};

#endif //INFORMATICA_GRAFICA_PHOTON_HPP
