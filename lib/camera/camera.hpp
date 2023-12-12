//
// camera.hpp
//
// Description:
//  Simple pinhole camera implementation
//
//
// Authors:
//  Samuel García
//  Laura González
//
// Date:
//  10/2022.
//

#ifndef INFORMATICA_GRAFICA_CAMERA_HPP
#define INFORMATICA_GRAFICA_CAMERA_HPP

#include "../math/Direction.hpp"
#include "../math/Point.hpp"
#include "../Ray.hpp"
#include <vector>

class Camera {
public:
    Direction L, U, F;
    Point O;
    size_t width, height;
    size_t rays_per_pixel;

private:
    static std::mt19937 gen;
    static std::uniform_real_distribution<double> pixel_distr;

public:

    // Look_at model with configurable vertical FOV (in degrees)
    Camera(Point _O, Point look_at, size_t _width, size_t _height, size_t _rays_per_pixel, double vertical_fov) :
        O(_O),
        width(_width),
        height(_height),
        rays_per_pixel(_rays_per_pixel) {

        vertical_fov = vertical_fov * (M_PI / 180); // Paso a radianes
        double aspect_ratio = ((double) _width / (double) _height);

        F.v = Vector3d(look_at.v - _O.v);
        L.v = (F.v * Vector3d(0, 1, 0)).normalize() * aspect_ratio;
        U.v = (F.v * Vector3d(1, 0, 0)).normalize();

        double U_mod = (tan(vertical_fov/2)*F.v.modulus());
        U.v = U.v * U_mod;

        double vertical_FOV = 2 * atan(U.v.modulus()/F.v.modulus());
        double horizontal_FOV = 2 * atan(L.v.modulus()/F.v.modulus());
        std::cout << "Look_at camera, vectors: L" << L.v << ", U" << U.v << ", F" << F.v << std::endl;
        std::cout << "Vertical FOV: " << vertical_FOV * 180/M_PI << std::endl;
        std::cout << "Horizontal FOV: " << horizontal_FOV * 180/M_PI << std::endl;
        std::cout << "Aspect ratio: " << width/height << std::endl;
    }


    // Look_at model without adjustable FOV
    Camera(Point _O, Point look_at, size_t _width, size_t _height, size_t _rays_per_pixel) :
        O(_O),
        width(_width),
        height(_height),
        rays_per_pixel(_rays_per_pixel) {

        double aspect_ratio = ((double) _width / (double) _height);

        F.v = Vector3d(look_at.v - _O.v);
        L.v = (F.v * Vector3d(0, 1, 0)).normalize() * aspect_ratio;
        U.v = (F.v * Vector3d(1, 0, 0)).normalize();

        double vertical_FOV = 2 * atan(U.v.modulus()/F.v.modulus());
        double horizontal_FOV = 2 * atan(L.v.modulus()/F.v.modulus());
        std::cout << "Look_at camera, vectors: L" << L.v << ", U" << U.v << ", F" << F.v << std::endl;
        std::cout << "Vertical FOV: " << vertical_FOV * 180/M_PI << std::endl;
        std::cout << "Horizontal FOV: " << horizontal_FOV * 180/M_PI << std::endl;
        std::cout << "Aspect ratio: " << width/height << std::endl;
    }


    // Manual model without aspect ratio calculations
    Camera(Point _O, Direction _L, Direction _U, Direction _F, size_t _width, size_t _height, size_t _rays_per_pixel) :
        L(_L),
        U(_U),
        F(_F),
        O(_O),
        width(_width),
        height(_height),
        rays_per_pixel(_rays_per_pixel) {

        double vertical_FOV = 2 * atan(U.v.modulus()/F.v.modulus());
        double horizontal_FOV = 2 * atan(L.v.modulus()/F.v.modulus());
        std::cout << "Manual camera, vectors: L" << L.v << ", U" << U.v << ", F" << F.v << std::endl;
        std::cout << "Vertical FOV: " << vertical_FOV * 180/M_PI << std::endl;
        std::cout << "Horizontal FOV: " << horizontal_FOV * 180/M_PI << std::endl;
        std::cout << "Aspect ratio: " << width/height << std::endl;
    }


    // Manual model without aspect ratio calculations
    Camera(Point _O, Direction _U, Direction _F, size_t _width, size_t _height, size_t _rays_per_pixel) :
            U(_U),
            F(_F),
            O(_O),
            width(_width),
            height(_height),
            rays_per_pixel(_rays_per_pixel) {
        // L es un vector perpendicular a F y U, cuya longitud es el aspect_ratio para que sea "panorámico"

        double aspect_ratio = ((double) _width / (double) _height);

        L = (F.v * U.v).normalize() * aspect_ratio;
        std::cout << "Panoramic image, vectors: L" << L.v << ", U" << U.v << ", F" << F.v << std::endl;

        double vertical_FOV = 2 * atan(U.v.modulus()/F.v.modulus());
        double horizontal_FOV = 2 * atan(L.v.modulus()/F.v.modulus());

        std::cout << "Manual aspect ratio-aware camera, vectors: L" << L.v << ", U" << U.v << ", F" << F.v << std::endl;
        std::cout << "Vertical FOV: " << vertical_FOV * 180/M_PI << std::endl;
        std::cout << "Horizontal FOV: " << horizontal_FOV * 180/M_PI << std::endl;
        std::cout << "Aspect ratio: " << width/height << std::endl;
    }

    [[nodiscard]] Ray get_ray(size_t _i, size_t _j) const {
        double i = (double) _i + 0.5 + pixel_distr(gen);
        double j = (double) _j + 0.5 + pixel_distr(gen);

        return {
            O,

            Direction((U.v + F.v + L.v
                    - ((2*L.v)/((double) width) * j)
                    - ((2*U.v)/((double) height) * i)))
        };
    }
};

#endif //INFORMATICA_GRAFICA_CAMERA_HPP
