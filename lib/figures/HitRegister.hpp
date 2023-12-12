//  
// HitRegister.hpp
// 
// Description:
//  Ray-figure collision register, returned by the hits function
//
//  Depending on the figure collided, some fields may have invalid contents
// 
//
// Authors:
//  Samuel García
//  Laura González
//
// Date:
//  10/2022.
//

#ifndef INFORMATICA_GRAFICA_HITREGISTER_HPP
#define INFORMATICA_GRAFICA_HITREGISTER_HPP

#include "../Ray.hpp"


class HitRegister {
public:
    bool hits{false}, is_area_light{false};

    // Smallest t value in the ray's hit equation o + d*t. In the case of multiple collisions,, t_max will be filled
    double t{std::numeric_limits<double>::min()}, t_max{std::numeric_limits<double>::min()}, refraction_index{0.0};

    // Surface normal
    Ray n{};

    Vector3d diffuse_coefficient{}, refraction_coefficient{}, reflection_coefficient{}, emission{};


    HitRegister() : t(std::numeric_limits<double>::min()), t_max(std::numeric_limits<double>::min()) {}

    HitRegister(bool _hits, double _t, Ray _n, Vector3d _diffuse_coefficient) : hits(_hits), t(_t), diffuse_coefficient(_diffuse_coefficient) {
        n = _n;
    }
};

#endif //INFORMATICA_GRAFICA_HITREGISTER_HPP
