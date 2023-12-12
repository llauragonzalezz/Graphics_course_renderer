//
// Ppm.hpp
//
// Description:
//  PPM file reader and writer, with auxiliary tonemapping functions
//
// Authors:
//  Samuel García
//  Laura González
//
// Date:
//  10/2022.
//

#ifndef INFORMATICA_GRAFICA_PPM_HPP
#define INFORMATICA_GRAFICA_PPM_HPP

#include <utility>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include "math/Vector3d.hpp" // Interpretado como tripletas rgb


class Ppm {
public:
    std::vector<Vector3d> img;

    // Max R/G/B value found
    double max;

    int width, height;
    double color_resolution;

public:
    Ppm () : max(1.0), width(0), height(0), color_resolution(0) {}

    Ppm (std::vector<Vector3d> _img, int _width, int _height) :
            img(std::move(_img)), width(_width), height(_height) {}

    Ppm (std::vector<Vector3d> _img, double _max, int _width, int _height, int _color_resolution) :
        img(std::move(_img)), max(_max), width(_width), height(_height), color_resolution(_color_resolution) {}

    Ppm (std::vector<Vector3d> _img, double _max, int _width, int _height) :
            img(std::move(_img)), max(_max), width(_width), height(_height), color_resolution(0) {}

    Ppm (double _max, int _width, int _height, int _color_resolution) :
            max(_max), width(_width), height(_height), color_resolution(_color_resolution) {}

    void read(std::string file_name) {
        img.clear();
        std::ifstream infile(file_name);

        std::string line;
        std::getline(infile, line);
        bool ppm_binario = (line == "P6");

        if (!ppm_binario) read_plain_ppm(infile);
        else read_binary_ppm(infile);
    }

    void read_plain_ppm(std::ifstream &infile) {
        std::string line;
        double factor = 0;
        bool res_read = false;

        while (std::getline(infile, line)) {
            if (line.at(0) == '#') {
                if (line.length() >= 4 && line.at(0) == '#' && line.at(1) == 'M' && line.at(2) == 'A' && line.at(3) == 'X') {
                    max = stod(line.substr(line.find('=') + 1, line.length()));
                }
            } else {
                if (!res_read) {
                    std::istringstream iss(line);
                    iss >> width >> height;
                    res_read = true;
                } else {
                    std::istringstream iss(line);
                    iss >> color_resolution;
                    factor = max / color_resolution;

                    while (std::getline(infile, line)) {
                        if (line.at(0) != '#') {
                            std::istringstream iss(line);
                            int r, g, b;
                            while (iss >> r >> g >> b) {
                                img.emplace_back(r * factor, g * factor, b * factor);
                            }
                            break;
                        }
                    }
                    break;
                }
            }
        }

        while (std::getline(infile, line)) {
            std::istringstream iss(line);
            int r, g, b;
            while (iss >> r >> g >> b) {
                img.emplace_back(r * factor, g * factor, b * factor);
            }
        }
    }

    void read_binary_ppm(std::ifstream &infile) {
        std::string line;

        std::getline(infile, line);
        std::istringstream iss(line);
        iss >> width >> height;

        std::getline(infile, line);
        std::istringstream iss2(line);
        iss2 >> color_resolution;

        line = "";
        std::string temp_line;
        int filas =0;
        while(std::getline(infile, temp_line)) {
            line += (temp_line+'\n');
            filas++;
        }

        std::byte bytes[line.size()];
        for (size_t i = 0; i < line.size(); i++) {
            bytes[i] = std::byte(line[i]);
        }

        bool r_read = false;
        bool g_read = false;
        double r,g,b;
        if (color_resolution < 256) {
            for (auto &byte : bytes) {
                if (!r_read) {
                    r = static_cast<double>(byte);

                    r_read = true;
                } else if (!g_read) {
                    g = static_cast<double>(byte);

                    g_read = true;
                } else {
                    b = static_cast<double>(byte);
                    img.emplace_back(r, g, b);

                    r_read = false;
                    g_read = false;
                }
            }
        } else {
            throw std::runtime_error("High-res PPM reading is not implemented");
        }
    }

    void write(std::string nombFich) const {
        double factor = color_resolution / max;

        std::ofstream outfile(nombFich);
        outfile << std::fixed << std::setprecision(0);
        outfile << "P3" << std::endl;
        outfile << "# " << nombFich << std::endl;
        outfile << "#MAX=" << max << std::endl;
        outfile << width << " " << height << std::endl;
        outfile << color_resolution << std::endl;

        int i = 0;
        for (auto v : img) {
            outfile << v[0]*factor << " " << v[1]*factor << " " << v[2]*factor << " ";

            i++;
            if (i == width - 1) {i = 0; outfile << std::endl;}
        }
    }

    void write_ldr(std::string nombFich) const {
        auto color_resolution_ldr = 65535;
        double factor = color_resolution_ldr / max;

        std::ofstream outfile(nombFich);
        outfile << std::fixed << std::setprecision(0);
        outfile << "P3" << std::endl;
        outfile << "# " << nombFich << std::endl;
        outfile << "#MAX=" << max << std::endl;
        outfile << width << " " << height << std::endl;
        outfile << color_resolution_ldr << std::endl;

        int i = 0;
        for (auto v : img) {
            outfile << v[0]*factor << " " << v[1]*factor << " " << v[2]*factor << " ";

            i++;
            if (i == width - 1) {i = 0; outfile << std::endl;}
        }
    }

    void write_hdr(std::string nombFich) const {
        auto color_resolution_hdr = std::pow(2, 30);
        auto max_hdr = max_rgb_value();

        double factor = color_resolution_hdr / max_hdr;

        std::ofstream outfile(nombFich);
        outfile << "P3" << std::endl;
        outfile << "# " << nombFich << std::endl;
        outfile << "#MAX=" << max_hdr << std::endl;
        outfile << width << " " << height << std::endl;
        outfile << color_resolution_hdr << std::endl;
        outfile << std::fixed << std::setprecision(0);

        int i = 0;
        for (auto v : img) {
            // Very simple firefly prevention
            double r = v[0]*factor < 0 ? -v[0]*factor : v[0]*factor;
            double g = v[1]*factor < 0 ? -v[1]*factor : v[1]*factor;
            double b = v[2]*factor < 0 ? -v[2]*factor : v[2]*factor;
            outfile << r << " " << g << " " << b << " ";

            i++;
            if (i == width - 1) {i = 0; outfile << std::endl;}
        }
    }

    [[nodiscard]] double max_rgb_value() const {
        auto max = 0.0;
        for (auto v : img) {
            max = std::max(std::max(std::max(v[0], v[1]), v[2]), max);
        }

        return max;
    }

    [[nodiscard]] double max_v_value() const {
        double max = 0.0;
        for (auto v : img) {
            max = std::max(v[2], max);
        }

        return max;
    }

    void rgbs_to_internal() {
        for (auto v : img) {
            v[0] /= 255.0;
            v[1] /= 255.0;
            v[2] /= 255.0;
        }
    }

};

#endif //INFORMATICA_GRAFICA_PPM_HPP
