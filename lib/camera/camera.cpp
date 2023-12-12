//  
// camera.cpp
// 
// Description:
//  Implementación de una cámara pinhole
// 
//
// Authors:
//  Samuel García
//  Laura González
//
// Date:
//  11/2022.
//

#include <random>
#include "camera.hpp"


std::mt19937 Camera::gen = std::mt19937((std::random_device()()));
std::uniform_real_distribution<double> Camera::pixel_distr = std::uniform_real_distribution<double>(-0.49, 0.49);