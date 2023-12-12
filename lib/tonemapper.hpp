//  
// tonemapper.hpp
// 
// Description:
//  PPM-based HSV and RGB tonemapper
//
// Authors:
//  Samuel García
//  Laura González
//
// Date:
//  10/2022.
//

#ifndef INFORMATICA_GRAFICA_TONEMAPPER_HPP
#define INFORMATICA_GRAFICA_TONEMAPPER_HPP

#include <cmath>
#include <utility>
#include <thread>
#include <cmath>
#include "Ppm.hpp"

class ToneMapper {
public:
    static Ppm equalize(const Ppm &ppm) {
        Ppm result(ppm.max, ppm.width, ppm.height, ppm.color_resolution);
        double max_r = 0.0, max_g = 0.0, max_b = 0.0;

        for (auto v : ppm.img) {
            if (v.c[0] > max_r){
                max_r = v.c[0];
            }

            if (v.c[1] > max_g){
                max_g = v.c[1];
            }

            if (v.c[2] > max_b){
                max_b = v.c[2];
            }
        }

        auto max_rgb = std::max(std::max(max_r, max_g),max_b);

        for (auto v : ppm.img) {
            result.img.emplace_back(Vector3d(v.c[0]/max_rgb, v.c[1]/max_rgb, v.c[2]/max_rgb));
        }

        result.max = 1.0;

        return result;
    }

    static Ppm equalize_hsv(const Ppm &ppm) {
        Ppm result(ppm.max, ppm.width, ppm.height, ppm.color_resolution);
        double max = 0.0;

        for (auto v : ppm.img) {
            max = std::max(max, v[2]);
        }

        for (auto v : ppm.img) {
            result.img.emplace_back(Vector3d(v.c[0], v.c[1], v.c[2]/max));
        }

        result.max = 1.0;

        return result;
    }

    static Ppm clamp(const Ppm &ppm, const double clamp_value) {
        Ppm result(ppm.max, ppm.width, ppm.height, ppm.color_resolution);

        for (auto v : ppm.img) {
            result.img.emplace_back(Vector3d( (v.c[0]>clamp_value)?clamp_value:v.c[0], (v.c[1]>clamp_value)?clamp_value:v.c[1], (v.c[2]>clamp_value)?clamp_value:v.c[2]));
        }

        return result;
    }

    static Ppm clamp_hsv(const Ppm &ppm, const double clamp_value) {
        Ppm result(ppm.max, ppm.width, ppm.height, ppm.color_resolution);

        for (auto v : ppm.img) {
            result.img.emplace_back(Vector3d(v.c[0], v.c[1], (v.c[2]>clamp_value)?clamp_value:v.c[2]));
        }

        return result;
    }

    static Ppm clamp_and_gamma_curve(const Ppm &ppm, const double clamp_value, const double gamma_value) {
        Ppm result = equalize(clamp(ppm, clamp_value));

        for (auto &v : result.img) {
            v.c[0] = pow(v.c[0], gamma_value);
            v.c[1] = pow(v.c[1], gamma_value);
            v.c[2] = pow(v.c[2], gamma_value);
        }
        return result;
    }

    static Ppm clamp_and_gamma_curve_hsv(const Ppm &ppm, const double clamp_value, const double gamma_value) {
        auto ppm_hsv = Ppm(ppm);

        for ( auto &v : ppm_hsv.img) {
            v = rgb_to_hsv(v);
        }

        Ppm result = equalize_hsv(clamp_hsv(ppm_hsv, clamp_value));

        for (auto &v : result.img) {
            v.c[2] = pow(v.c[2], gamma_value); // Aplica la curva solo sobre V
        }

        for ( auto &v : result.img) {
            v = hsv_to_rgb(v);
        }

        return result;
    }

    static std::pair<Ppm, Ppm> tone_map_rgb_and_hsv(const Ppm &ppm) {
        return std::make_pair(clamp_and_gamma_curve(ppm, ppm.max_rgb_value()*0.8, 0.5),
                              clamp_and_gamma_curve_hsv(ppm, ppm.max_v_value()*0.8, 0.5));
    }



    // Tonemapping parameter testing functions
private:
    // Based on:
    //  https://stackoverflow.com/questions/3018313/algorithm-to-convert-rgb-to-hsv-and-hsv-to-rgb-in-range-0-255-for-both
    static Vector3d rgb_to_hsv(const Vector3d &rgb) {
        double h, s, v;

        auto max = std::max(std::max(rgb[0], rgb[1]), rgb[2]);
        auto min = std::min(std::min(rgb[0], rgb[1]), rgb[2]);

        v = max;
        auto delta = max - min;
        if (delta < 0.00001) {
            return {0, 0, v};
        }

        if ( max > 0.0 ) {
            s = (delta / max);
        } else {
            return {NAN, 0, v};
        }
        if (rgb[0] >= max)
            h = ( rgb[1] - rgb[2] ) / delta;
        else
        if( rgb[1] >= max )
            h = 2.0 + ( rgb[2] - rgb[0] ) / delta;
        else
            h = 4.0 + ( rgb[0] - rgb[1] ) / delta;

        h *= 60.0;

        if( h < 0.0 )
            h += 360.0;

        return {h*M_PI/180, s, v};
    }

    // Based on:
    //  https://stackoverflow.com/questions/3018313/algorithm-to-convert-rgb-to-hsv-and-hsv-to-rgb-in-range-0-255-for-both
    static Vector3d hsv_to_rgb(const Vector3d &hsv) {
        double r, g, b;
        double      hh, p, q, t, ff;
        long        i;

        if(hsv[1] <= 0.0) {
            r = hsv[2];
            g = hsv[2];
            b = hsv[2];
            return {r, g, b};
        }
        hh = hsv[0];
        if(hh >= 360.0*M_PI/180) hh = 0.0;
        hh /= 60.0*M_PI/180;
        i = (long)hh;
        ff = hh - i;
        p = hsv[2] * (1.0 - hsv[1]);
        q = hsv[2] * (1.0 - (hsv[1] * ff));
        t = hsv[2] * (1.0 - (hsv[1] * (1.0 - ff)));

        switch(i) {
            case 0:
                r = hsv[2];
                g = t;
                b = p;
                break;
            case 1:
                r = q;
                g = hsv[2];
                b = p;
                break;
            case 2:
                r = p;
                g = hsv[2];
                b = t;
                break;

            case 3:
                r = p;
                g = q;
                b = hsv[2];
                break;
            case 4:
                r = t;
                g = p;
                b = hsv[2];
                break;
            case 5:
            default:
                r = hsv[2];
                g = p;
                b = q;
                break;
        }

        return {r, g, b};
    }
};


#endif //INFORMATICA_GRAFICA_TONEMAPPER_HPP
