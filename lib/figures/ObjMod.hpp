//
// TriangleMesh.hpp
//
// Description:
//  Simple OBJ file vertex-to-triangles converter
//
//
// Authors:
//  Samuel García
//  Laura González
//
// Date:
//  10/2022.
//

#ifndef INFORMATICA_GRAFICA_OBJMOD_HPP
#define INFORMATICA_GRAFICA_OBJMOD_HPP
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <regex>
#include "Triangle.hpp"

// Textured
std::vector<Triangle> load_obj(std::string filename,
                               Vector3d tr_diffuse_coefficient,
                               Vector3d tr_refraction_coefficient,
                               Vector3d tr_reflection_coefficient,
                               double tr_refraction_index,
                               std::shared_ptr<Texture> _texture) {
    std::vector<Vector3d> v;
    std::vector<Vector3d> vt;
    std::vector<Vector3d> vn;
    std::vector<Triangle> f;

    std::ifstream infile(filename);
    std::string line, s;
    double x, y, z, vtx, vty, vtz, vnx, vny, vnz;

    std::cout << "Loading textured .obj (it might take a minute...)" << std::endl;

    while (std::getline(infile, line)) {
        std::istringstream iss(line);
        iss >> s;
        if (s == "v") { // Vertex
            iss >> x >> y >> z;
            v.emplace_back(x, y, z);
        } else if (s == "vt") { // Texture
            iss >> x >> y;
            vt.emplace_back(x, y, 0);
        } else if (s == "vn") { // Normal
            iss >> x >> y >> z;
            vn.emplace_back(x, y, z);
        } else if (s == "f") { // Figure
            std::regex pattern1(R"(f (\d+)/(\d+)/(\d+) (\d+)/(\d+)/(\d+) (\d+)/(\d+)/(\d+))"); // Vertex + Texture + Normal
            std::regex pattern2(R"(f (\d+)//(\d+) (\d+)//(\d+) (\d+)//(\d+))"); // Vertex + Normal
            std::regex pattern3(R"(f (\d+)/(\d+) (\d+)/(\d+) (\d+)/(\d+))"); // Vertex + Texture
            std::regex pattern4(R"(f (\d+) (\d+) (\d+))"); // Vertex

            std::smatch matches;
            if (std::regex_search(line, matches, pattern1)) { // We ignore vn's
                x = std::stoi(matches[1]); vtx = std::stoi(matches[2]); vnx = std::stoi(matches[3]);
                y = std::stoi(matches[4]); vty = std::stoi(matches[5]); vny = std::stoi(matches[6]);
                z = std::stoi(matches[7]); vtz = std::stoi(matches[8]); vnz = std::stoi(matches[9]);
                f.emplace_back(Point(v[x - 1]),
                               Point(v[y - 1]),
                               Point(v[z - 1]),
                               tr_diffuse_coefficient,
                               tr_refraction_coefficient,
                               tr_reflection_coefficient,
                               tr_refraction_index, _texture,
                               Point(vt[vtx - 1]),
                               Point(vt[vty - 1]),
                               Point(vt[vtz - 1]));

            } else if (std::regex_search(line, matches, pattern2)) {
                x = std::stoi(matches[1]); vnx = std::stoi(matches[2]);
                y = std::stoi(matches[3]); vny = std::stoi(matches[4]);
                z = std::stoi(matches[5]); vnz = std::stoi(matches[6]);
                f.emplace_back(Point(v[x - 1]),
                               Point(v[y - 1]),
                               Point(v[z - 1]),
                               tr_diffuse_coefficient,
                               tr_refraction_coefficient,
                               tr_reflection_coefficient,
                               tr_refraction_index);

            } else if (std::regex_search(line, matches, pattern3)) {
                x = std::stoi(matches[1]); vtx = std::stoi(matches[2]);
                y = std::stoi(matches[3]); vty = std::stoi(matches[4]);
                z = std::stoi(matches[5]); vtz = std::stoi(matches[6]);
                f.emplace_back(Point(v[x - 1]),
                               Point(v[y - 1]),
                               Point(v[z - 1]),
                               tr_diffuse_coefficient,
                               tr_refraction_coefficient,
                               tr_reflection_coefficient,
                               tr_refraction_index, _texture,
                               Point(vt[vtx - 1]),
                               Point(vt[vty - 1]),
                               Point(vt[vtz - 1]));

            } else if (std::regex_search(line, matches, pattern4)) {
                x = std::stoi(matches[1]);
                y = std::stoi(matches[2]);
                z = std::stoi(matches[3]);
                f.emplace_back(Point(v[x - 1]),
                               Point(v[y - 1]),
                               Point(v[z - 1]),
                               tr_diffuse_coefficient,
                               tr_refraction_coefficient,
                               tr_reflection_coefficient,
                               tr_refraction_index);
            }
        }
    }

    std::cout  << "Finished loading textured .obj" << std::endl;
    return f;
}

// Untextured
std::vector<Triangle> load_obj(std::string filename,
                               Vector3d tr_diffuse_coefficient,
                               Vector3d tr_refraction_coefficient,
                               Vector3d tr_reflection_coefficient,
                               double tr_refraction_index) {
    std::cout << "Loading untextured .obj" << std::endl;

    std::vector<Vector3d> v;
    std::vector<Vector3d> vt;
    std::vector<Vector3d> vn;
    std::vector<Triangle> f;

    std::ifstream infile(filename);
    std::string line, s, v1, v2, v3;
    double x, y, z;

    while (std::getline(infile, line)) {
        std::istringstream iss(line);
        iss >> s;
        if (s == "v") { // Vertex
            iss >> x >> y >> z;
            v.emplace_back(x, y, z);
        } else if (s == "f") { // Figure
            iss >> v1 >> v2 >> v3;

            f.emplace_back(Point(v[stoi(v1.substr(0, s.find('/'))) - 1]),
                           Point(v[stoi(v2.substr(0, s.find('/'))) - 1]),
                           Point(v[stoi(v3.substr(0, s.find('/'))) - 1]),
                           tr_diffuse_coefficient,
                           tr_refraction_coefficient,
                           tr_reflection_coefficient,
                           tr_refraction_index);
        }
    }

    std::cout << "Finished loading untextured .obj" << std::endl;
    return f;
}
#endif //INFORMATICA_GRAFICA_OBJMOD_HPP
