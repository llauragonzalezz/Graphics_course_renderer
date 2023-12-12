//  
// Texture.hpp
// 
// Description:
//  Texture implementation with support for (u,v) indexing
// 
//
// Authors:
//  Samuel García
//  Laura González
//
// Date:
//  11/2022.
//

#ifndef INFORMATICA_GRAFICA_TEXTURE_HPP
#define INFORMATICA_GRAFICA_TEXTURE_HPP

#include <utility>

#include "../lib/Ppm.hpp"
#include "../lib/tonemapper.hpp"

class Texture {
public:
    Ppm texture_image;

    Texture() {}

    Texture(std::string file_name) {
        texture_image.read(std::move(file_name));

        // Fixes some badly illuminated textures
        texture_image = ToneMapper::clamp(ToneMapper::equalize(texture_image), 0.75);
    }

    /*
     * (v)
     *  ^           (1,1)
     *  |
     *  |
     *  | (0,0)
     *  ---------------> (u)
     *
     * */
    Vector3d get_texel(double u, double v) const {
        size_t x = (size_t)(u * (texture_image.width - 1)) % texture_image.width;
        size_t y = (size_t)(v * (texture_image.height - 1)) % texture_image.height;

        size_t px = y * texture_image.height + x;

        return texture_image.img.at(px);
    }
};

#endif //INFORMATICA_GRAFICA_TEXTURE_HPP
