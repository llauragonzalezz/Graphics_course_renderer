//  
// Scene.hpp
// 
// Description:
//      Testing collection of scenes. Warning: this is messy! Some scenes look for textures not included in Github
//
// Authors:
//  Samuel García
//  Laura González
//
// Date:
//  11/2022.
//

#ifndef INFORMATICA_GRAFICA_SCENE_HPP
#define INFORMATICA_GRAFICA_SCENE_HPP

#include <memory>
#include <utility>
#include <vector>
#include "Figure.hpp"
#include "PointLight.hpp"
#include "camera.hpp"
#include "ObjMod.hpp"
#include "Sphere.hpp"
#include "Cylinder.hpp"
#include "Cone.hpp"
#include "Ellipsoid.hpp"
#include "Disk.hpp"
#include "TransformedFigure.hpp"
#include "ConstructiveSolidUnion.hpp"
#include "ConstructiveSolidDifference.hpp"

class Scene {
public:
    std::vector<std::shared_ptr<Figure>> figures;
    std::vector<PointLight> point_lights;
    Camera camera;

    explicit Scene(std::vector<std::shared_ptr<Figure>> _figures, std::vector<PointLight> _point_lights, Camera _camera) : figures(std::move(_figures)), point_lights(std::move(_point_lights)), camera(_camera) {}

    static Scene cornell_box_area_light(size_t width, size_t height, size_t rays_per_pixel) {
        Point O(0, 0, -3.5);
        Direction U(0, 1, 0);
        Direction F(0, 0, 3);

        Camera camera(O, U, F, width, height, rays_per_pixel);
        //Camera camera(O, U, F, width, height, rays_per_pixel);

        Plane left_plane(1, Direction(1, 0, 0), Vector3d(0.8, 0, 0), Vector3d(0, 0, 0), Vector3d(0, 0, 0), 1.0);
        Plane right_plane(1, Direction(-1, 0, 0), Vector3d(0, 0.8, 0), Vector3d(0, 0, 0), Vector3d(0, 0, 0), 1.0);
        Plane floor_plane(1, Direction(0, 1, 0), Vector3d(0.6, 0.6, 0.6), Vector3d(0, 0, 0), Vector3d(0, 0, 0), 1.0);
        Plane ceiling_plane(1, Direction(0, -1, 0), Vector3d(1, 1, 1)); // Luz de área
        Plane back_plane(1, Direction(0, 0, -1), Vector3d(0.6, 0.6, 0.6), Vector3d(0, 0, 0), Vector3d(0, 0, 0), 1.0);

        // dif + spec
        Sphere s_left(Point(-0.5,-0.7, 0.25), 0.3, Vector3d(0.3, 0.5, 0.7), Vector3d(0, 0, 0), Vector3d(0.25, 0.25, 0.25), 1.0);

        // refr + spec
        Sphere s_right(Point(0.5,-0.7,-0.25), 0.3, Vector3d(0, 0, 0), Vector3d(0.85, 0.85, 0.85), Vector3d(0.1, 0.1, 0.1), 1.5); // Cristal

        std::vector<std::shared_ptr<Figure>> figures;
        figures.push_back(std::make_shared<Sphere>(s_right));
        figures.push_back(std::make_shared<Sphere>(s_left));
        figures.push_back(std::make_shared<Plane>(left_plane));
        figures.push_back(std::make_shared<Plane>(right_plane));
        figures.push_back(std::make_shared<Plane>(floor_plane));
        figures.push_back(std::make_shared<Plane>(ceiling_plane));
        figures.push_back(std::make_shared<Plane>(back_plane));

        // Una luz puntual debajo de la luz de área
        std::vector<PointLight> point_lights;
        //point_lights.emplace_back(Point(0, 0.5, 0), Vector3d(1, 1, 1));

        return Scene(figures, point_lights, camera);
    }

    static Scene cornell_box_two_point_lights(size_t width, size_t height, size_t rays_per_pixel) {
        Point O(0, 0, -3.5);
        Direction U(0, 1, 0);
        Direction F(0, 0, 3);

        Camera camera(O, U, F, width, height, rays_per_pixel);
        //Camera camera(O, U, F, width, height, rays_per_pixel);

        Plane left_plane(1, Direction(1, 0, 0), Vector3d(0.8, 0, 0), Vector3d(0, 0, 0), Vector3d(0, 0, 0), 1.0);
        Plane right_plane(1, Direction(-1, 0, 0), Vector3d(0, 0.8, 0), Vector3d(0, 0, 0), Vector3d(0, 0, 0), 1.0);
        Plane floor_plane(1, Direction(0, 1, 0), Vector3d(0.6, 0.6, 0.6), Vector3d(0, 0, 0), Vector3d(0, 0, 0), 1.0);
        Plane ceiling_plane(1, Direction(0, -1, 0), Vector3d(0.5, 0.5, 0.5)); // Luz de área
        Plane back_plane(1, Direction(0, 0, -1), Vector3d(0.6, 0.6, 0.6), Vector3d(0, 0, 0), Vector3d(0, 0, 0), 1.0);

        // dif + spec
        Sphere s_left(Point(-0.5,-0.7, 0.25), 0.3, Vector3d(0.3, 0.5, 0.7), Vector3d(0, 0, 0), Vector3d(0.25, 0.25, 0.25), 1.0);

        // refr + spec
        Sphere s_right(Point(0.5,-0.7,-0.25), 0.3, Vector3d(0, 0, 0), Vector3d(0.85, 0.85, 0.85), Vector3d(0.1, 0.1, 0.1), 1.5); // Cristal

        std::vector<std::shared_ptr<Figure>> figures;
        figures.push_back(std::make_shared<Sphere>(s_right));
        figures.push_back(std::make_shared<Sphere>(s_left));
        figures.push_back(std::make_shared<Plane>(left_plane));
        figures.push_back(std::make_shared<Plane>(right_plane));
        figures.push_back(std::make_shared<Plane>(floor_plane));
        figures.push_back(std::make_shared<Plane>(ceiling_plane));
        figures.push_back(std::make_shared<Plane>(back_plane));

        // Una luz puntual debajo de la luz de área
        std::vector<PointLight> point_lights;
        point_lights.emplace_back(Point(0, 0.5, 0), Vector3d(0.5, 0.5, 0.5));

        return Scene(figures, point_lights, camera);
    }

    static Scene cornell_box_many_point_lights(size_t width, size_t height, size_t rays_per_pixel) {
        Point O(0, 0, -3.5);
        Direction U(0, 1, 0);
        Direction F(0, 0, 3);

        Camera camera(O, U, F, width, height, rays_per_pixel);
        //Camera camera(O, U, F, width, height, rays_per_pixel);

        Plane left_plane(1, Direction(1, 0, 0), Vector3d(0.8, 0, 0), Vector3d(0, 0, 0), Vector3d(0, 0, 0), 1.0);
        Plane right_plane(1, Direction(-1, 0, 0), Vector3d(0, 0.8, 0), Vector3d(0, 0, 0), Vector3d(0, 0, 0), 1.0);
        Plane floor_plane(1, Direction(0, 1, 0), Vector3d(0.6, 0.6, 0.6), Vector3d(0, 0, 0), Vector3d(0, 0, 0), 1.0);
        Plane ceiling_plane(1, Direction(0, -1, 0), Vector3d(0.6, 0.6, 0.6), Vector3d(0, 0, 0), Vector3d(0, 0, 0), 1.0);
        Plane back_plane(1, Direction(0, 0, -1), Vector3d(0.6, 0.6, 0.6), Vector3d(0, 0, 0), Vector3d(0, 0, 0), 1.0);

        // dif + spec
        Sphere s_left(Point(-0.5,-0.7, 0.25), 0.3, Vector3d(0.3 / 3, 0.5 / 3, 0.7 / 3), Vector3d(0, 0, 0), Vector3d(0.8, 0.8, 0.8), 1.0);

        // refr + spec
        Sphere s_right(Point(0.5,-0.7,-0.25), 0.3, Vector3d(0, 0, 0), Vector3d(0.85, 0.85, 0.85), Vector3d(0.1, 0.1, 0.1), 1.5); // Cristal

        std::vector<std::shared_ptr<Figure>> figures;
        figures.push_back(std::make_shared<Sphere>(s_right));
        figures.push_back(std::make_shared<Sphere>(s_left));
        figures.push_back(std::make_shared<Plane>(left_plane));
        figures.push_back(std::make_shared<Plane>(right_plane));
        figures.push_back(std::make_shared<Plane>(floor_plane));
        figures.push_back(std::make_shared<Plane>(ceiling_plane));
        figures.push_back(std::make_shared<Plane>(back_plane));

        // Una luz puntual debajo de la luz de área
        std::vector<PointLight> point_lights;
        // y = 0.1
        point_lights.emplace_back(Point(-0.8, 0.1, 0), Vector3d(0.5, 0.5, 0.5));
        point_lights.emplace_back(Point(-0.65, 0.1, 0), Vector3d(0.5, 0.5, 0.5));
        point_lights.emplace_back(Point(0.55, 0.1, 0), Vector3d(0.5, 0.5, 0.5));
        point_lights.emplace_back(Point(0.65, 0.1, 0), Vector3d(0.5, 0.5, 0.5));
        point_lights.emplace_back(Point(0.8, 0.1, 0), Vector3d(0.5, 0.5, 0.5));

         // y = 0.2
        point_lights.emplace_back(Point(-0.6, 0.2, 0), Vector3d(0.5, 0.5, 0.5));
        point_lights.emplace_back(Point(-0.3, 0.2, 0), Vector3d(0.5, 0.5, 0.5));
        point_lights.emplace_back(Point(0.2, 0.2, 0), Vector3d(0.5, 0.5, 0.5));
        point_lights.emplace_back(Point(0.3, 0.2, 0), Vector3d(0.5, 0.5, 0.5));
        point_lights.emplace_back(Point(0.6, 0.2, 0), Vector3d(0.5, 0.5, 0.5));

        // y = 0.3
        point_lights.emplace_back(Point(-0.7, 0.3, 0), Vector3d(0.5, 0.5, 0.5));
        point_lights.emplace_back(Point(-0.55, 0.3, 0), Vector3d(0.5, 0.5, 0.5));
        point_lights.emplace_back(Point(0.4, 0.3, 0), Vector3d(0.5, 0.5, 0.5));
        point_lights.emplace_back(Point(0.55, 0.3, 0), Vector3d(0.5, 0.5, 0.5));
        point_lights.emplace_back(Point(0.7, 0.3, 0), Vector3d(0.5, 0.5, 0.5));

        // y = 0.4
        point_lights.emplace_back(Point(-0.9, 0.4, 0), Vector3d(0.5, 0.5, 0.5));
        point_lights.emplace_back(Point(-0.84, 0.4, 0), Vector3d(0.5, 0.5, 0.5));
        point_lights.emplace_back(Point(0.72, 0.4, 0), Vector3d(0.5, 0.5, 0.5));
        point_lights.emplace_back(Point(0.84, 0.4, 0), Vector3d(0.5, 0.5, 0.5));
        point_lights.emplace_back(Point(0.9, 0.4, 0), Vector3d(0.5, 0.5, 0.5));

        // y = 0.5
        point_lights.emplace_back(Point(-0.5, 0.5, 0), Vector3d(0.5, 0.5, 0.5));
        point_lights.emplace_back(Point(-0.25, 0.5, 0), Vector3d(0.5, 0.5, 0.5));
        point_lights.emplace_back(Point(0, 0.5, 0), Vector3d(0.5, 0.5, 0.5));
        point_lights.emplace_back(Point(0.25, 0.5, 0), Vector3d(0.5, 0.5, 0.5));
        point_lights.emplace_back(Point(0.5, 0.5, 0), Vector3d(0.5, 0.5, 0.5));

        // y = 0.6
        point_lights.emplace_back(Point(-0.87, 0.6, 0), Vector3d(0.5, 0.5, 0.5));
        point_lights.emplace_back(Point(-0.73, 0.6, 0), Vector3d(0.5, 0.5, 0.5));
        point_lights.emplace_back(Point(0.6, 0.6, 0), Vector3d(0.5, 0.5, 0.5));
        point_lights.emplace_back(Point(0.73, 0.6, 0), Vector3d(0.5, 0.5, 0.5));
        point_lights.emplace_back(Point(0.87, 0.6, 0), Vector3d(0.5, 0.5, 0.5));

        // y = 0.7
        point_lights.emplace_back(Point(-0.45, 0.7, 0), Vector3d(0.5, 0.5, 0.5));
        point_lights.emplace_back(Point(-0.35, 0.7, 0), Vector3d(0.5, 0.5, 0.5));
        point_lights.emplace_back(Point(0.15, 0.7, 0), Vector3d(0.5, 0.5, 0.5));
        point_lights.emplace_back(Point(0.35, 0.7, 0), Vector3d(0.5, 0.5, 0.5));
        point_lights.emplace_back(Point(0.45, 0.7, 0), Vector3d(0.5, 0.5, 0.5));



        return Scene(figures, point_lights, camera);
    }

    static Scene cornell_box_refractive_sphere(size_t width, size_t height, size_t rays_per_pixel) {
        Point O(0, 0, -3.5);
        Direction U(0, 1, 0);
        Direction F(0, 0, 3);

        Camera camera(O, U, F, width, height, rays_per_pixel);
        //Camera camera(O, U, F, width, height, rays_per_pixel);

        Plane left_plane(1, Direction(1, 0, 0), Vector3d(0.8, 0, 0), Vector3d(0, 0, 0), Vector3d(0, 0, 0), 1.0);
        Plane right_plane(1, Direction(-1, 0, 0), Vector3d(0, 0.8, 0), Vector3d(0, 0, 0), Vector3d(0, 0, 0), 1.0);
        Plane floor_plane(1, Direction(0, 1, 0), Vector3d(0.6, 0.6, 0.6), Vector3d(0, 0, 0), Vector3d(0, 0, 0), 1.0);
        Plane ceiling_plane(1, Direction(0, -1, 0), Vector3d(1, 1, 1)); // Luz de área
        Plane back_plane(1, Direction(0, 0, -1), Vector3d(0.6, 0.6, 0.6), Vector3d(0, 0, 0), Vector3d(0, 0, 0), 1.0);

        Sphere s(Point(0, -0.7, 0.25), 0.3, Vector3d(0, 0, 0), Vector3d(0.85, 0.85, 0.85), Vector3d(0, 0, 0), 1.5);


        std::vector<std::shared_ptr<Figure>> figures;
        figures.push_back(std::make_shared<Sphere>(s));
        figures.push_back(std::make_shared<Plane>(left_plane));
        figures.push_back(std::make_shared<Plane>(right_plane));
        figures.push_back(std::make_shared<Plane>(floor_plane));
        figures.push_back(std::make_shared<Plane>(ceiling_plane));
        figures.push_back(std::make_shared<Plane>(back_plane));

        // Una luz puntual debajo de la luz de área
        std::vector<PointLight> point_lights;
        //point_lights.emplace_back(Point(0, 0.5, 0), Vector3d(1, 1, 1));

        return Scene(figures, point_lights, camera);
    }

    static Scene cornell_box_reflective_sphere(size_t width, size_t height, size_t rays_per_pixel) {
        Point O(0, 0, -3.5);
        Direction U(0, 1, 0);
        Direction F(0, 0, 3);

        Camera camera(O, U, F, width, height, rays_per_pixel);
        //Camera camera(O, U, F, width, height, rays_per_pixel);

        Plane left_plane(1, Direction(1, 0, 0), Vector3d(0.8, 0, 0), Vector3d(0, 0, 0), Vector3d(0, 0, 0), 1.0);
        Plane right_plane(1, Direction(-1, 0, 0), Vector3d(0, 0.8, 0), Vector3d(0, 0, 0), Vector3d(0, 0, 0), 1.0);
        Plane floor_plane(1, Direction(0, 1, 0), Vector3d(0.6, 0.6, 0.6), Vector3d(0, 0, 0), Vector3d(0, 0, 0), 1.0);
        Plane ceiling_plane(1, Direction(0, -1, 0), Vector3d(1, 1, 1)); // Luz de área
        Plane back_plane(1, Direction(0, 0, -1), Vector3d(0.6, 0.6, 0.6), Vector3d(0, 0, 0), Vector3d(0, 0, 0), 1.0);

        Sphere s(Point(0, -0.7, 0.25), 0.3, Vector3d(0, 0, 0), Vector3d(0, 0, 0), Vector3d(0.85, 0.85, 0.85), 1.0);


        std::vector<std::shared_ptr<Figure>> figures;
        figures.push_back(std::make_shared<Sphere>(s));
        figures.push_back(std::make_shared<Plane>(left_plane));
        figures.push_back(std::make_shared<Plane>(right_plane));
        figures.push_back(std::make_shared<Plane>(floor_plane));
        figures.push_back(std::make_shared<Plane>(ceiling_plane));
        figures.push_back(std::make_shared<Plane>(back_plane));

        // Una luz puntual debajo de la luz de área
        std::vector<PointLight> point_lights;
        //point_lights.emplace_back(Point(0, 0.5, 0), Vector3d(1, 1, 1));

        return Scene(figures, point_lights, camera);
    }

    static Scene cornell_box_diffuse_sphere(size_t width, size_t height, size_t rays_per_pixel) {
        Point O(0, 0, -3.5);
        Direction U(0, 1, 0);
        Direction F(0, 0, 3);

        Camera camera(O, U, F, width, height, rays_per_pixel);
        //Camera camera(O, U, F, width, height, rays_per_pixel);

        Plane left_plane(1, Direction(1, 0, 0), Vector3d(0.8, 0, 0), Vector3d(0, 0, 0), Vector3d(0, 0, 0), 1.0);
        Plane right_plane(1, Direction(-1, 0, 0), Vector3d(0, 0.8, 0), Vector3d(0, 0, 0), Vector3d(0, 0, 0), 1.0);
        Plane floor_plane(1, Direction(0, 1, 0), Vector3d(0.6, 0.6, 0.6), Vector3d(0, 0, 0), Vector3d(0, 0, 0), 1.0);
        Plane ceiling_plane(1, Direction(0, -1, 0), Vector3d(1, 1, 1)); // Luz de área
        Plane back_plane(1, Direction(0, 0, -1), Vector3d(0.6, 0.6, 0.6), Vector3d(0, 0, 0), Vector3d(0, 0, 0), 1.0);

        Sphere s(Point(0, -0.7, 0.25), 0.3, Vector3d(0.8, 0, 0.8), Vector3d(0, 0, 0), Vector3d(0, 0, 0), 1.0);


        std::vector<std::shared_ptr<Figure>> figures;
        figures.push_back(std::make_shared<Sphere>(s));
        figures.push_back(std::make_shared<Plane>(left_plane));
        figures.push_back(std::make_shared<Plane>(right_plane));
        figures.push_back(std::make_shared<Plane>(floor_plane));
        figures.push_back(std::make_shared<Plane>(ceiling_plane));
        figures.push_back(std::make_shared<Plane>(back_plane));

        // Una luz puntual debajo de la luz de área
        std::vector<PointLight> point_lights;
        //point_lights.emplace_back(Point(0, 0.5, 0), Vector3d(1, 1, 1));

        return Scene(figures, point_lights, camera);
    }

    static Scene cornell_box_refractive_sphere_point_light(size_t width, size_t height, size_t rays_per_pixel) {
        Point O(0, 0, -3.5);
        Direction U(0, 1, 0);
        Direction F(0, 0, 3);

        Camera camera(O, U, F, width, height, rays_per_pixel);
        //Camera camera(O, U, F, width, height, rays_per_pixel);

        Plane left_plane(1, Direction(1, 0, 0), Vector3d(0.8, 0, 0), Vector3d(0, 0, 0), Vector3d(0, 0, 0), 1.0);
        Plane right_plane(1, Direction(-1, 0, 0), Vector3d(0, 0.8, 0), Vector3d(0, 0, 0), Vector3d(0, 0, 0), 1.0);
        Plane floor_plane(1, Direction(0, 1, 0), Vector3d(0.6, 0.6, 0.6), Vector3d(0, 0, 0), Vector3d(0, 0, 0), 1.0);
        Plane ceiling_plane(1, Direction(0, -1, 0), Vector3d(0.6, 0.6, 0.6), Vector3d(0, 0, 0), Vector3d(0, 0, 0), 1.0);
        Plane back_plane(1, Direction(0, 0, -1), Vector3d(0.6, 0.6, 0.6), Vector3d(0, 0, 0), Vector3d(0, 0, 0), 1.0);

        Sphere s(Point(0, -0.7, 0.25), 0.3, Vector3d(0, 0, 0), Vector3d(0.85, 0.85, 0.85), Vector3d(0, 0, 0), 1.5);


        std::vector<std::shared_ptr<Figure>> figures;
        figures.push_back(std::make_shared<Sphere>(s));
        figures.push_back(std::make_shared<Plane>(left_plane));
        figures.push_back(std::make_shared<Plane>(right_plane));
        figures.push_back(std::make_shared<Plane>(floor_plane));
        figures.push_back(std::make_shared<Plane>(ceiling_plane));
        figures.push_back(std::make_shared<Plane>(back_plane));

        // Una luz puntual debajo de la luz de área
        std::vector<PointLight> point_lights;
        point_lights.emplace_back(Point(0, 0.5, 0), Vector3d(1, 1, 1));

        return Scene(figures, point_lights, camera);
    }

    static Scene cornell_box_reflective_sphere_point_light(size_t width, size_t height, size_t rays_per_pixel) {
        Point O(0, 0, -3.5);
        Direction U(0, 1, 0);
        Direction F(0, 0, 3);

        Camera camera(O, U, F, width, height, rays_per_pixel);
        //Camera camera(O, U, F, width, height, rays_per_pixel);

        Plane left_plane(1, Direction(1, 0, 0), Vector3d(0.8, 0, 0), Vector3d(0, 0, 0), Vector3d(0, 0, 0), 1.0);
        Plane right_plane(1, Direction(-1, 0, 0), Vector3d(0, 0.8, 0), Vector3d(0, 0, 0), Vector3d(0, 0, 0), 1.0);
        Plane floor_plane(1, Direction(0, 1, 0), Vector3d(0.6, 0.6, 0.6), Vector3d(0, 0, 0), Vector3d(0, 0, 0), 1.0);
        Plane ceiling_plane(1, Direction(0, -1, 0), Vector3d(0.6, 0.6, 0.6), Vector3d(0, 0, 0), Vector3d(0, 0, 0), 1.0);
        Plane back_plane(1, Direction(0, 0, -1), Vector3d(0.6, 0.6, 0.6), Vector3d(0, 0, 0), Vector3d(0, 0, 0), 1.0);

        Sphere s(Point(0, -0.7, 0.25), 0.3, Vector3d(0, 0, 0), Vector3d(0, 0, 0), Vector3d(0.85, 0.85, 0.85), 1.0);


        std::vector<std::shared_ptr<Figure>> figures;
        figures.push_back(std::make_shared<Sphere>(s));
        figures.push_back(std::make_shared<Plane>(left_plane));
        figures.push_back(std::make_shared<Plane>(right_plane));
        figures.push_back(std::make_shared<Plane>(floor_plane));
        figures.push_back(std::make_shared<Plane>(ceiling_plane));
        figures.push_back(std::make_shared<Plane>(back_plane));

        // Una luz puntual debajo de la luz de área
        std::vector<PointLight> point_lights;
        point_lights.emplace_back(Point(0, 0.5, 0), Vector3d(1, 1, 1));

        return Scene(figures, point_lights, camera);
    }

    static Scene cornell_box_diffuse_sphere_point_light(size_t width, size_t height, size_t rays_per_pixel) {
        Point O(0, 0, -3.5);
        Direction U(0, 1, 0);
        Direction F(0, 0, 3);

        Camera camera(O, U, F, width, height, rays_per_pixel);
        //Camera camera(O, U, F, width, height, rays_per_pixel);

        Plane left_plane(1, Direction(1, 0, 0), Vector3d(0.8, 0, 0), Vector3d(0, 0, 0), Vector3d(0, 0, 0), 1.0);
        Plane right_plane(1, Direction(-1, 0, 0), Vector3d(0, 0.8, 0), Vector3d(0, 0, 0), Vector3d(0, 0, 0), 1.0);
        Plane floor_plane(1, Direction(0, 1, 0), Vector3d(0.6, 0.6, 0.6), Vector3d(0, 0, 0), Vector3d(0, 0, 0), 1.0);
        Plane ceiling_plane(1, Direction(0, -1, 0), Vector3d(0.6, 0.6, 0.6), Vector3d(0, 0, 0), Vector3d(0, 0, 0), 1.0);
        Plane back_plane(1, Direction(0, 0, -1), Vector3d(0.6, 0.6, 0.6), Vector3d(0, 0, 0), Vector3d(0, 0, 0), 1.0);

        Sphere s(Point(0, -0.7, 0.25), 0.3, Vector3d(0.8, 0, 0.8), Vector3d(0, 0, 0), Vector3d(0, 0, 0), 1.0);


        std::vector<std::shared_ptr<Figure>> figures;
        figures.push_back(std::make_shared<Sphere>(s));
        figures.push_back(std::make_shared<Plane>(left_plane));
        figures.push_back(std::make_shared<Plane>(right_plane));
        figures.push_back(std::make_shared<Plane>(floor_plane));
        figures.push_back(std::make_shared<Plane>(ceiling_plane));
        figures.push_back(std::make_shared<Plane>(back_plane));

        // Una luz puntual debajo de la luz de área
        std::vector<PointLight> point_lights;
        point_lights.emplace_back(Point(0, 0.5, 0), Vector3d(1, 1, 1));

        return Scene(figures, point_lights, camera);
    }

    static Scene cornell_box_figuritas(size_t width, size_t height, size_t rays_per_pixel) {
        Point O(0, 0, -3.5);
        Direction U(0, 1, 0);
        Direction F(0, 0, 3);
        Camera camera(O, U, F, width, height, rays_per_pixel);
        //Camera camera(O, U, F, width, height, rays_per_pixel);

        Plane left_plane(1, Direction(1, 0, 0), Vector3d(0.8, 0, 0), Vector3d(0, 0, 0), Vector3d(0, 0, 0), 1.0);
        Plane right_plane(1, Direction(-1, 0, 0), Vector3d(0, 0.8, 0), Vector3d(0, 0, 0), Vector3d(0, 0, 0), 1.0);
        Plane floor_plane(1, Direction(0, 1, 0), Vector3d(0.6, 0.6, 0.6), Vector3d(0, 0, 0), Vector3d(0, 0, 0), 1.0);
        Plane ceiling_plane(1, Direction(0, -1, 0), Vector3d(1, 1, 1)); // Luz de área
        Plane back_plane(1, Direction(0, 0, -1), Vector3d(0.6, 0.6, 0.6), Vector3d(0, 0, 0), Vector3d(0, 0, 0), 1.0);

        Vector3d tr_diffuse_coefficient(0.3, 0.5, 0.7);
        Vector3d tr_refraction_coefficient(0, 0, 0);
        Vector3d tr_reflection_coefficient(1, 1, 1);
        double tr_refraction_index = 1;

        // dif + spec
        Sphere s_left(Point(-.4,.3, -0.25), 0.25,
                      Vector3d(0.3, 0.5, 0.7),
                      tr_refraction_coefficient,
                      tr_reflection_coefficient,
                      tr_refraction_index);

        Ellipsoid ellipsoid(Point(.95,-1.2, -0.25), 0.25, 0.35, 0.75,
                      Vector3d(0.3, 0.5, 0.7),
                      tr_refraction_coefficient,
                      tr_reflection_coefficient,
                      tr_refraction_index);

        Cylinder cilindro(Point(0.8,-0.4,-0.15), 0.3, 0.4,
                          Vector3d(0.3, 0.5, 0.7),
                          tr_refraction_coefficient,
                          tr_reflection_coefficient,
                          tr_refraction_index);

        TransformationMatrix m = TransformationMatrix::RotationMatrixOnY(M_PI/2);
        TransformationMatrix m1 = TransformationMatrix::RotationMatrixOnZ(M_PI/2);

        TransformedFigure cilindroGirado = TransformedFigure(m, std::make_shared<Cylinder>(cilindro),
                                              Vector3d(0.3, 0.5, 0.7),
                                            tr_refraction_coefficient,
                                            tr_reflection_coefficient,
                                                tr_refraction_index);

        Cone cono(Point(-0.5,-0.3, 0.55), 0.5,
                  Vector3d(0.3, 0.5, 0.7),
                  tr_refraction_coefficient,
                  tr_reflection_coefficient,
                  tr_refraction_index);

        TransformedFigure conoGirado = TransformedFigure(m1*m, std::make_shared<Cone>(cono),
                Vector3d(0.3, 0.3, 0.7),
                tr_refraction_coefficient,
                tr_reflection_coefficient,
                tr_refraction_index);


        std::vector<std::shared_ptr<Figure>> figures;

        figures.push_back(std::make_shared<TransformedFigure>(cilindroGirado));
        figures.push_back(std::make_shared<TransformedFigure>(conoGirado));
        figures.push_back(std::make_shared<Ellipsoid>(ellipsoid));
        figures.push_back(std::make_shared<Sphere>(s_left));
        figures.push_back(std::make_shared<Plane>(left_plane));
        figures.push_back(std::make_shared<Plane>(right_plane));
        figures.push_back(std::make_shared<Plane>(floor_plane));
        figures.push_back(std::make_shared<Plane>(ceiling_plane));
        figures.push_back(std::make_shared<Plane>(back_plane));

        // Una luz puntual debajo de la luz de área
        std::vector<PointLight> point_lights;
        //point_lights.emplace_back(Point(0, 0.5, 0), Vector3d(1, 1, 1));

        return Scene(figures, point_lights, camera);
    }

    static Scene cornell_box_elipsoide(size_t width, size_t height, size_t rays_per_pixel) {
        Point O(0, 0, -3.5);
        Direction U(0, 1, 0);
        Direction F(0, 0, 3);
        Camera camera(O, U, F, width, height, rays_per_pixel);
        //Camera camera(O, U, F, width, height, rays_per_pixel);

        Plane left_plane(1, Direction(1, 0, 0), Vector3d(0.8, 0, 0), Vector3d(0, 0, 0), Vector3d(0, 0, 0), 1.0);
        Plane right_plane(1, Direction(-1, 0, 0), Vector3d(0, 0.8, 0), Vector3d(0, 0, 0), Vector3d(0, 0, 0), 1.0);
        Plane floor_plane(1, Direction(0, 1, 0), Vector3d(0.6, 0.6, 0.6), Vector3d(0, 0, 0), Vector3d(0, 0, 0), 1.0);
        Plane ceiling_plane(1, Direction(0, -1, 0), Vector3d(1, 1, 1)); // Luz de área
        Plane back_plane(1, Direction(0, 0, -1), Vector3d(0.6, 0.6, 0.6), Vector3d(0, 0, 0), Vector3d(0, 0, 0), 1.0);

        Vector3d tr_diffuse_coefficient(0.3, 0.5, 0.7);
        Vector3d tr_refraction_coefficient(0, 0, 0);
        Vector3d tr_reflection_coefficient(1, 1, 1);
        double tr_refraction_index = 1;

        Ellipsoid ellipsoid1(Point(-1.7,-1, -0.25), 0.25, 0.35, 0.75,
                            Vector3d(0.3, 0.5, 0.7),
                            tr_refraction_coefficient,
                            tr_reflection_coefficient,
                            tr_refraction_index);

        Ellipsoid ellipsoid2(Point(0.8,-1.5, -0.25), 0.38, 0.25, 0.75,
                             Vector3d(0.3, 0.5, 0.7),
                             tr_refraction_coefficient,
                             tr_reflection_coefficient,
                             tr_refraction_index);

        std::vector<std::shared_ptr<Figure>> figures;
        figures.push_back(std::make_shared<Ellipsoid>(ellipsoid1));
        figures.push_back(std::make_shared<Ellipsoid>(ellipsoid2));
        figures.push_back(std::make_shared<Plane>(left_plane));
        figures.push_back(std::make_shared<Plane>(right_plane));
        figures.push_back(std::make_shared<Plane>(floor_plane));
        figures.push_back(std::make_shared<Plane>(ceiling_plane));
        figures.push_back(std::make_shared<Plane>(back_plane));

        // Una luz puntual debajo de la luz de área
        std::vector<PointLight> point_lights;
        //point_lights.emplace_back(Point(0, 0.5, 0), Vector3d(1, 1, 1));

        return Scene(figures, point_lights, camera);
    }

    static Scene cornell_box_cilindro(size_t width, size_t height, size_t rays_per_pixel) {
        Point O(0, 0, -3.5);
        Direction U(0, 1, 0);
        Direction F(0, 0, 3);
        Camera camera(O, U, F, width, height, rays_per_pixel);
        //Camera camera(O, U, F, width, height, rays_per_pixel);

        Plane left_plane(1, Direction(1, 0, 0), Vector3d(0.8, 0, 0), Vector3d(0, 0, 0), Vector3d(0, 0, 0), 1.0);
        Plane right_plane(1, Direction(-1, 0, 0), Vector3d(0, 0.8, 0), Vector3d(0, 0, 0), Vector3d(0, 0, 0), 1.0);
        Plane floor_plane(1, Direction(0, 1, 0), Vector3d(0.6, 0.6, 0.6), Vector3d(0, 0, 0), Vector3d(0, 0, 0), 1.0);
        Plane ceiling_plane(1, Direction(0, -1, 0), Vector3d(1, 1, 1)); // Luz de área
        Plane back_plane(1, Direction(0, 0, -1), Vector3d(0.6, 0.6, 0.6), Vector3d(0, 0, 0), Vector3d(0, 0, 0), 1.0);

        Vector3d tr_diffuse_coefficient(0.3, 0.5, 0.7);
        Vector3d tr_refraction_coefficient(0, 0, 0);
        Vector3d tr_reflection_coefficient(1, 1, 1);
        double tr_refraction_index = 1;

        Cylinder cilindro(Point(0,0,0.5), 0.3, 1,
                          Vector3d(0.3, 0.5, 0.7),
                          tr_refraction_coefficient,
                          tr_reflection_coefficient,
                          tr_refraction_index);

        TransformationMatrix m = TransformationMatrix::RotationMatrixOnX(M_PI/2);

        TransformedFigure cilindroGirado = TransformedFigure(m, std::make_shared<Cylinder>(cilindro),
                                                             Vector3d(0.3, 0.5, 0.7),
                                                             tr_refraction_coefficient,
                                                             tr_reflection_coefficient,
                                                             tr_refraction_index);


        std::vector<std::shared_ptr<Figure>> figures;
        figures.push_back(std::make_shared<TransformedFigure>(cilindroGirado));
        figures.push_back(std::make_shared<Plane>(left_plane));
        figures.push_back(std::make_shared<Plane>(right_plane));
        figures.push_back(std::make_shared<Plane>(floor_plane));
        figures.push_back(std::make_shared<Plane>(ceiling_plane));
        figures.push_back(std::make_shared<Plane>(back_plane));

        // Una luz puntual debajo de la luz de área
        std::vector<PointLight> point_lights;
        //point_lights.emplace_back(Point(0, 0.5, 0), Vector3d(1, 1, 1));

        return Scene(figures, point_lights, camera);
    }

    static Scene cornell_box_cono(size_t width, size_t height, size_t rays_per_pixel) {
        Point O(0, 0, -3.5);
        Direction U(0, 1, 0);
        Direction F(0, 0, 3);
        Camera camera(O, U, F, width, height, rays_per_pixel);
        //Camera camera(O, U, F, width, height, rays_per_pixel);

        Plane left_plane(1, Direction(1, 0, 0), Vector3d(0.8, 0, 0), Vector3d(0, 0, 0), Vector3d(0, 0, 0), 1.0);
        Plane right_plane(1, Direction(-1, 0, 0), Vector3d(0, 0.8, 0), Vector3d(0, 0, 0), Vector3d(0, 0, 0), 1.0);
        Plane floor_plane(1, Direction(0, 1, 0), Vector3d(0.6, 0.6, 0.6), Vector3d(0, 0, 0), Vector3d(0, 0, 0), 1.0);
        Plane ceiling_plane(1, Direction(0, -1, 0), Vector3d(1, 1, 1)); // Luz de área
        Plane back_plane(1, Direction(0, 0, -1), Vector3d(0.6, 0.6, 0.6), Vector3d(0, 0, 0), Vector3d(0, 0, 0), 1.0);

        Vector3d tr_diffuse_coefficient(0.3, 0.5, 0.7);
        Vector3d tr_refraction_coefficient(0, 0, 0);
        Vector3d tr_reflection_coefficient(1, 1, 1);
        double tr_refraction_index = 1;

        Cone cono(Point(0,0,0.5), .8,
                          Vector3d(0.3, 0.5, 0.7),
                          tr_refraction_coefficient,
                          tr_reflection_coefficient,
                          tr_refraction_index);

        TransformationMatrix m = TransformationMatrix::RotationMatrixOnX(M_PI/2);
        m = m * TransformationMatrix::RotationMatrixOnY(M_PI) ;
        TransformedFigure conoGirado = TransformedFigure(m, std::make_shared<Cone>(cono),
                                                             Vector3d(0.3, 0.5, 0.7),
                                                             tr_refraction_coefficient,
                                                             tr_reflection_coefficient,
                                                             tr_refraction_index);

        std::vector<std::shared_ptr<Figure>> figures;
        figures.push_back(std::make_shared<TransformedFigure>(conoGirado));
        figures.push_back(std::make_shared<Plane>(left_plane));
        figures.push_back(std::make_shared<Plane>(right_plane));
        figures.push_back(std::make_shared<Plane>(floor_plane));
        figures.push_back(std::make_shared<Plane>(ceiling_plane));
        figures.push_back(std::make_shared<Plane>(back_plane));

        // Una luz puntual debajo de la luz de área
        std::vector<PointLight> point_lights;
        //point_lights.emplace_back(Point(0, 0.5, 0), Vector3d(1, 1, 1));

        return Scene(figures, point_lights, camera);
    }

    static Scene cornell_box_circulo(size_t width, size_t height, size_t rays_per_pixel) {
        Point O(0, 0, -3.5);
        Direction U(0, 1, 0);
        Direction F(0, 0, 3);
        Camera camera(O, U, F, width, height, rays_per_pixel);
        //Camera camera(O, U, F, width, height, rays_per_pixel);

        Plane left_plane(1, Direction(1, 0, 0), Vector3d(0.8, 0, 0), Vector3d(0, 0, 0), Vector3d(0, 0, 0), 1.0);
        Plane right_plane(1, Direction(-1, 0, 0), Vector3d(0, 0.8, 0), Vector3d(0, 0, 0), Vector3d(0, 0, 0), 1.0);
        Plane floor_plane(1, Direction(0, 1, 0), Vector3d(0.6, 0.6, 0.6), Vector3d(0, 0, 0), Vector3d(0, 0, 0), 1.0);
        Plane ceiling_plane(1, Direction(0, -1, 0), Vector3d(1, 1, 1)); // Luz de área
        Plane back_plane(1, Direction(0, 0, -1), Vector3d(0.6, 0.6, 0.6), Vector3d(0, 0, 0), Vector3d(0, 0, 0), 1.0);

        Vector3d tr_diffuse_coefficient(0.3, 0.5, 0.7);
        Vector3d tr_refraction_coefficient(0, 0, 0);
        Vector3d tr_reflection_coefficient(0, 0, 0);
        double tr_refraction_index = 1;

        Disk circulo(Point(0,0,0), .5,
                  Vector3d(0.3, 0.5, 0.7),
                  tr_refraction_coefficient,
                  tr_reflection_coefficient,
                  tr_refraction_index);

        std::vector<std::shared_ptr<Figure>> figures;
        figures.push_back(std::make_shared<Disk>(circulo));
        figures.push_back(std::make_shared<Plane>(left_plane));
        figures.push_back(std::make_shared<Plane>(right_plane));
        figures.push_back(std::make_shared<Plane>(floor_plane));
        figures.push_back(std::make_shared<Plane>(ceiling_plane));
        figures.push_back(std::make_shared<Plane>(back_plane));

        // Una luz puntual debajo de la luz de área
        std::vector<PointLight> point_lights;
        //point_lights.emplace_back(Point(0, 0.5, 0), Vector3d(1, 1, 1));

        return Scene(figures, point_lights, camera);
    }

    static Scene cornell_box_triangulo(size_t width, size_t height, size_t rays_per_pixel) {
        Point O(0, 0, -3.5);
        Direction U(0, 1, 0);
        Direction F(0, 0, 3);
        Camera camera(O, U, F, width, height, rays_per_pixel);
        //Camera camera(O, U, F, width, height, rays_per_pixel);

        Plane left_plane(1, Direction(1, 0, 0), Vector3d(0.8, 0, 0), Vector3d(0, 0, 0), Vector3d(0, 0, 0), 1.0);
        Plane right_plane(1, Direction(-1, 0, 0), Vector3d(0, 0.8, 0), Vector3d(0, 0, 0), Vector3d(0, 0, 0), 1.0);
        Plane floor_plane(1, Direction(0, 1, 0), Vector3d(0.6, 0.6, 0.6), Vector3d(0, 0, 0), Vector3d(0, 0, 0), 1.0);
        Plane ceiling_plane(1, Direction(0, -1, 0), Vector3d(1, 1, 1)); // Luz de área
        Plane back_plane(1, Direction(0, 0, -1), Vector3d(0.6, 0.6, 0.6), Vector3d(0, 0, 0), Vector3d(0, 0, 0), 1.0);

        Vector3d tr_diffuse_coefficient(0.5, 0.5, 0.5);
        Vector3d tr_refraction_coefficient(0, 0, 0);
        Vector3d tr_reflection_coefficient(0, 0, 0);
        double tr_refraction_index = 1;

        Triangle triangulo(Point(-0.5,-0.7,0.25),
                           Point(0.25,0.7,0.25),
                           Point(0.5,-0.7,-0.25),
                           Vector3d(0.3, 0.5, 0.7),
                           tr_refraction_coefficient,
                           tr_reflection_coefficient,
                           tr_refraction_index);

        std::vector<std::shared_ptr<Figure>> figures;
        figures.push_back(std::make_shared<Triangle>(triangulo));
        figures.push_back(std::make_shared<Plane>(left_plane));
        figures.push_back(std::make_shared<Plane>(right_plane));
        figures.push_back(std::make_shared<Plane>(floor_plane));
        figures.push_back(std::make_shared<Plane>(ceiling_plane));
        figures.push_back(std::make_shared<Plane>(back_plane));

        // Una luz puntual debajo de la luz de área
        std::vector<PointLight> point_lights;
        //point_lights.emplace_back(Point(0, 0.5, 0), Vector3d(1, 1, 1));

        return Scene(figures, point_lights, camera);
    }

    static Scene cornell_box_texturas(size_t width, size_t height, size_t rays_per_pixel) {
        Point O(0, 0, -3.5);

        // Equivalentes en 16:9
        //Camera camera(O, Point(Vector3d(0, 0, -0.5)), width, height, rays_per_pixel, 36.86);
        Camera camera(O, Point(Vector3d(0, 0, -0.5)), width, height, rays_per_pixel);

        std::shared_ptr<Texture> test_texture = std::make_shared<Texture>("texture_files/test_texture.ppm");
        std::shared_ptr<Texture> chevron_texture = std::make_shared<Texture>("texture_files/chevron_3.ppm");
        std::shared_ptr<Texture> earth_texture = std::make_shared<Texture>("texture_files/earth.ppm");
        std::shared_ptr<Texture> brick_texture = std::make_shared<Texture>("texture_files/ladrillos.ppm");


        Plane left_plane(1, Direction(1, 0, 0), Vector3d(0.8, 0, 0), Vector3d(0, 0, 0), Vector3d(0, 0, 0), 1.0, test_texture);
        Plane right_plane(1, Direction(-1, 0, 0), Vector3d(0, 0.8, 0), Vector3d(0, 0, 0), Vector3d(0, 0, 0), 1.0, test_texture);
        Plane floor_plane(1, Direction(0, 1, 0), Vector3d(0.6, 0.6, 0.6), Vector3d(0, 0, 0), Vector3d(0, 0, 0), 1.0, chevron_texture);
        Plane ceiling_plane(1, Direction(0, -1, 0), Vector3d(1, 1, 1)); // Luz de área
        Plane back_plane(1, Direction(0, 0, -1), Vector3d(0.6, 0.6, 0.6), Vector3d(0, 0, 0), Vector3d(0, 0, 0), 1.0, brick_texture);

        Sphere s_left(Point(-0.5,-0.7,0.25), 0.3,  Vector3d(0.8, 0, 0.8), Vector3d(0, 0, 0), Vector3d(0.0, 0.0, 0.0), 1.0, earth_texture);
        Sphere s_right(Point(0.5,-0.7,-0.25), 0.3, Vector3d(0, 0, 0), Vector3d(0.8, 0.8, 0.8), Vector3d(0, 0, 0), 1.5); // Cristal

        std::vector<std::shared_ptr<Figure>> figures;
        figures.push_back(std::make_shared<Sphere>(s_right));
        figures.push_back(std::make_shared<Sphere>(s_left));
        figures.push_back(std::make_shared<Plane>(left_plane));
        figures.push_back(std::make_shared<Plane>(right_plane));
        figures.push_back(std::make_shared<Plane>(floor_plane));
        figures.push_back(std::make_shared<Plane>(ceiling_plane));
        figures.push_back(std::make_shared<Plane>(back_plane));

        // Una luz puntual debajo de la luz de área
        std::vector<PointLight> point_lights;
        //point_lights.emplace_back(Point(0, 0.5, 0), Vector3d(1, 1, 1));

        return Scene(figures, point_lights, camera);
    }

    static Scene cornell_box_point_light(size_t width, size_t height, size_t rays_per_pixel) {
        Point O(0, 0, -3.5);
        Direction U(0, 1, 0);
        Direction F(0, 0, 3);

        Camera camera(O, U, F, width, height, rays_per_pixel);

        Plane left_plane(1, Direction(1, 0, 0), Vector3d(0.8, 0, 0), Vector3d(0, 0, 0), Vector3d(0, 0, 0), 1.0);
        Plane right_plane(1, Direction(-1, 0, 0), Vector3d(0, 0.8, 0), Vector3d(0, 0, 0), Vector3d(0, 0, 0), 1.0);
        Plane floor_plane(1, Direction(0, 1, 0), Vector3d(0.6, 0.6, 0.6), Vector3d(0, 0, 0), Vector3d(0, 0, 0), 1.0);
        Plane ceiling_plane(1, Direction(0, -1, 0), Vector3d(0.6, 0.6, 0.6), Vector3d(0, 0, 0), Vector3d(0, 0, 0), 1.0);
        Plane back_plane(1, Direction(0, 0, -1), Vector3d(0.6, 0.6, 0.6), Vector3d(0, 0, 0), Vector3d(0, 0, 0), 1.0);

        // Difuso, refr, refl

        // dif + spec
        Sphere s_left(Point(-0.5,-0.7, 0.25), 0.3, Vector3d(0.3 / 3, 0.5 / 3, 0.7 / 3), Vector3d(0, 0, 0), Vector3d(0.8, 0.8, 0.8), 1.0);

        // refr + spec
        Sphere s_right(Point(0.5,-0.7,-0.25), 0.3, Vector3d(0, 0, 0), Vector3d(0.85, 0.85, 0.85), Vector3d(0.1, 0.1, 0.1), 1.5); // Cristal

        std::vector<std::shared_ptr<Figure>> figures;
        figures.push_back(std::make_shared<Sphere>(s_right));
        figures.push_back(std::make_shared<Sphere>(s_left));
        figures.push_back(std::make_shared<Plane>(left_plane));
        figures.push_back(std::make_shared<Plane>(right_plane));
        figures.push_back(std::make_shared<Plane>(floor_plane));
        figures.push_back(std::make_shared<Plane>(ceiling_plane));
        figures.push_back(std::make_shared<Plane>(back_plane));

        // Una luz puntual debajo de la luz de área
        std::vector<PointLight> point_lights;
        point_lights.emplace_back(Point(0, 0.5, 0), Vector3d(1, 1, 1));

        return Scene(figures, point_lights, camera);
    }

    static Scene cornell_box_point_light_diffuse_spheres(size_t width, size_t height, size_t rays_per_pixel) {
        Point O(0, 0, -3.5);
        Direction U(0, 1, 0);
        Direction F(0, 0, 3);

        Camera camera(O, U, F, width, height, rays_per_pixel);

        Plane left_plane(1, Direction(1, 0, 0), Vector3d(0.8, 0, 0), Vector3d(0, 0, 0), Vector3d(0, 0, 0), 1.0);
        Plane right_plane(1, Direction(-1, 0, 0), Vector3d(0, 0.8, 0), Vector3d(0, 0, 0), Vector3d(0, 0, 0), 1.0);
        Plane floor_plane(1, Direction(0, 1, 0), Vector3d(0.6, 0.6, 0.6), Vector3d(0, 0, 0), Vector3d(0, 0, 0), 1.0);
        Plane ceiling_plane(1, Direction(0, -1, 0), Vector3d(0.6, 0.6, 0.6), Vector3d(0, 0, 0), Vector3d(0, 0, 0), 1.0);
        Plane back_plane(1, Direction(0, 0, -1), Vector3d(0.6, 0.6, 0.6), Vector3d(0, 0, 0), Vector3d(0, 0, 0), 1.0);

        // Difuso, refr, refl

        Sphere s_left(Point(-0.5,-0.7, 0.25), 0.3, Vector3d(0.8, 0.0, 0.8), Vector3d(0, 0, 0), Vector3d(0, 0, 0), 1.0);
        Sphere s_right(Point(0.5,-0.7,-0.25), 0.3, Vector3d(0, 0.8, 0.8), Vector3d(0, 0, 0), Vector3d(0, 0, 0), 1.5); // Cristal

        std::vector<std::shared_ptr<Figure>> figures;
        figures.push_back(std::make_shared<Sphere>(s_right));
        figures.push_back(std::make_shared<Sphere>(s_left));
        figures.push_back(std::make_shared<Plane>(left_plane));
        figures.push_back(std::make_shared<Plane>(right_plane));
        figures.push_back(std::make_shared<Plane>(floor_plane));
        figures.push_back(std::make_shared<Plane>(ceiling_plane));
        figures.push_back(std::make_shared<Plane>(back_plane));

        // Una luz puntual debajo de la luz de área
        std::vector<PointLight> point_lights;
        point_lights.emplace_back(Point(0, 0.5, 0), Vector3d(1, 1, 1));

        return Scene(figures, point_lights, camera);
    }

    static Scene cornell_box_twin_peaks(size_t width, size_t height, size_t rays_per_pixel) {
        Point O(2, 0.55, -3.5); // Un poco a la derecha, mirando desde arriba

        // Equivalentes en 16:9
        Camera camera(O, Point(Vector3d(0, -0.5, -0.5)), width, height, rays_per_pixel); // Hacia el centro, un poco hacia abajo

        std::shared_ptr<Texture> chevron_texture = std::make_shared<Texture>("texture_files/chevron_3.ppm");
        std::shared_ptr<Texture> venus_texture = std::make_shared<Texture>("texture_files/textura_venus.ppm");

        Plane floor_plane(1, Direction(0, 1.045, 0), Vector3d(0.6, 0.6, 0.6), Vector3d(0, 0, 0), Vector3d(0.2, 0.2, 0.2), 1.0, chevron_texture);
        Plane ceiling_plane_above_light(1, Direction(0, -1, 0), Vector3d(146.0/255.0, 0, 2.0/255.0), Vector3d(0, 0, 0), Vector3d(0, 0, 0), 1.0); // Luz de área
        Plane ceiling_plane(1, Direction(0, -1, 0), Vector3d(1, 1, 1)); // Luz de área
        ceiling_plane.set_bounds(Point(-0.5, 0.99, -0.75), Point(0.5, 1.01, std::numeric_limits<double>::max()));
        Plane back_plane(1, Direction(0, 0, -1), Vector3d(127.0 / 255.0, 42.0 / 255.0, 60.0 / 255.0), Vector3d(0, 0, 0), Vector3d(0, 0, 0), 1.0); // "Red velvet"

        Vector3d tr_diffuse_coefficient(0, 0, 0); // "Black"
        Vector3d tr_refraction_coefficient(0.1, 0.1, 0.1);
        Vector3d tr_reflection_coefficient(0.1, 0.1, 0.1);
        double tr_refraction_index = 1.56; // Softwood
        std::vector<std::shared_ptr<Figure>> figures;
        auto triangulos = load_obj("objs/final2/lamparamesilla.obj", Vector3d(189.0 / 255.0, 243.0 / 255.0, 226.0 / 255.0), Vector3d(0.15, 0.15, 0.15),
                                   Vector3d(0.15, 0.15, 0.15), tr_refraction_index);
        for (const auto& triangulo : triangulos) {
            figures.push_back(std::make_shared<Triangle>(triangulo));
        }

        triangulos = load_obj("objs/final2/sofa.obj", tr_diffuse_coefficient, tr_refraction_coefficient,
                                   tr_reflection_coefficient, tr_refraction_index);
        for (const auto& triangulo : triangulos) {
            figures.push_back(std::make_shared<Triangle>(triangulo));
        }

        tr_diffuse_coefficient = Vector3d(142.0 / 255.0, 142.0 / 255.0, 142.0 / 255.0); // "Gris metálico"
        tr_refraction_coefficient = Vector3d(0.15, 0.15, 0.15);
        tr_reflection_coefficient = Vector3d(0.25, 0.25, 0.25);
        tr_refraction_index = 2.6112; // Titanio
        triangulos = load_obj("objs/final2/lampara.obj", tr_diffuse_coefficient, tr_refraction_coefficient,
                                   tr_reflection_coefficient, tr_refraction_index);
        for (const auto& triangulo : triangulos) {
            figures.push_back(std::make_shared<Triangle>(triangulo));
        }

        tr_diffuse_coefficient = Vector3d(245 / 255.0, 247 / 255.0, 247 / 255.0); // "Marble"
        tr_refraction_coefficient = Vector3d(0.04, 0.04, 0.04);
        tr_reflection_coefficient = Vector3d(0.04, 0.04, 0.04);
        tr_refraction_index = 1.486; // Marble
        triangulos = load_obj("objs/final2/venus6.obj", tr_diffuse_coefficient, tr_refraction_coefficient,
                              tr_reflection_coefficient, tr_refraction_index, venus_texture);
        for (const auto& triangulo : triangulos)
            figures.push_back(std::make_shared<Triangle>(triangulo));


        tr_diffuse_coefficient = Vector3d(127.0 / 255.0, 42.0 / 255.0, 60.0 / 255.0); // "Red velvet"
        tr_refraction_coefficient = Vector3d(0, 0, 0);
        tr_reflection_coefficient = Vector3d(0, 0, 0);
        tr_refraction_index = 1;
        triangulos = load_obj("objs/final2/cortinas_todo.obj", tr_diffuse_coefficient, tr_refraction_coefficient,
                              tr_reflection_coefficient, tr_refraction_index);
        for (const auto& triangulo : triangulos)
            figures.push_back(std::make_shared<Triangle>(triangulo));


        tr_diffuse_coefficient = Vector3d(130.0 /255, 135.0 / 255, 135.0 / 255); // "Black"
        tr_refraction_coefficient = Vector3d(0.2, 0.2, 0.2);
        tr_reflection_coefficient = Vector3d(0.2, 0.2, 0.2);
        tr_refraction_index = 1.1978; // Aluminio
        triangulos = load_obj("objs/final2/mesilla.obj", tr_diffuse_coefficient, tr_refraction_coefficient,
                                   tr_reflection_coefficient, tr_refraction_index);
        for (const auto& triangulo : triangulos) {
            figures.push_back(std::make_shared<Triangle>(triangulo));
        }

        /*Cylinder cilindro(Point(1, 0, 1), 0.15, 0.6,
                          Vector3d(127.0 / 255.0, 42.0 / 255.0, 60.0 / 255.0),
                          tr_refraction_coefficient,
                          tr_reflection_coefficient,
                          tr_refraction_index);
        Cylinder cilindro2(Point(1, -0.70, 0.285), 0.15, 0.6,
                           Vector3d(127.0 / 255.0, 42.0 / 255.0, 60.0 / 255.0),
                           tr_refraction_coefficient,
                           tr_reflection_coefficient,
                           tr_refraction_index);
        TransformationMatrix m1 = TransformationMatrix::RotationMatrixOnY(M_PI/3);
        TransformationMatrix m2 = TransformationMatrix::RotationMatrixOnX(M_PI/2);
        TransformedFigure cilindroGirado1 = TransformedFigure(m2, std::make_shared<Cylinder>(cilindro),
                                                              Vector3d(127.0 / 255.0, 42.0 / 255.0, 60.0 / 255.0),
                                                              tr_refraction_coefficient,
                                                              tr_reflection_coefficient,
                                                              tr_refraction_index);


        ConstructiveSolidUnion unionCilindros(std::make_shared<TransformedFigure>(cilindroGirado1),std::make_shared<Cylinder>(cilindro2));

        TransformedFigure unionCilindrosGirados = TransformedFigure(m1, std::make_shared<ConstructiveSolidUnion>(unionCilindros),
                                                                    Vector3d(127.0 / 255.0, 42.0 / 255.0, 60.0 / 255.0),
                                                                    tr_refraction_coefficient,
                                                                    tr_reflection_coefficient,
                                                                    tr_refraction_index);*/

        //figures.push_back(std::make_shared<TransformedFigure>(unionCilindrosGirados));

        /*Sphere s(Point(0.48, -0.45, -0.80), 0.15, Vector3d(0, 0, 0), Vector3d(0.65, 0.65, 0.65), Vector3d(0.3, 0.3, 0.3), 1.5); // Cristal
        //figures.push_back(std::make_shared<Sphere>(s));

        Sphere s2(Point(0.7, -0.71, -0.68), 0.15, Vector3d(0, 0, 0), Vector3d(0.65, 0.65, 0.65), Vector3d(0.3, 0.3, 0.3), 1.5); // Cristal
        //figures.push_back(std::make_shared<Sphere>(s2));

        Sphere s3(Point(0.25, -0.72, -0.96), 0.15, Vector3d(0, 0, 0), Vector3d(0.65, 0.65, 0.65), Vector3d(0.3, 0.3, 0.3), 1.5); // Cristal
        //figures.push_back(std::make_shared<Sphere>(s3));

        ConstructiveSolidUnion unionConS1(std::make_shared<TransformedFigure>(unionCilindrosGirados), std::make_shared<Sphere>(s));
        ConstructiveSolidUnion unionConS2(std::make_shared<ConstructiveSolidUnion>(unionConS1), std::make_shared<Sphere>(s2));
        ConstructiveSolidUnion unionConS3(std::make_shared<ConstructiveSolidUnion>(unionConS2), std::make_shared<Sphere>(s3));

        TransformationMatrix escalado = TransformationMatrix::ScaleMatrix(0.8, 0.8, 0.8);
        TransformedFigure unionEscalada = TransformedFigure(escalado, std::make_shared<ConstructiveSolidUnion>(unionConS3),
                                                                    Vector3d(127.0 / 255.0, 42.0 / 255.0, 60.0 / 255.0),
                                                                    tr_refraction_coefficient,
                                                                    tr_reflection_coefficient,
                                                                    tr_refraction_index);
        figures.push_back(std::make_shared<TransformedFigure>(unionEscalada));*/

        //figures.push_back(std::make_shared<Plane>(left_plane));
        //figures.push_back(std::make_shared<Plane>(right_plane));
        figures.push_back(std::make_shared<Plane>(floor_plane));
        figures.push_back(std::make_shared<Plane>(ceiling_plane_above_light));
        figures.push_back(std::make_shared<Plane>(ceiling_plane));
        figures.push_back(std::make_shared<Plane>(back_plane));

        std::vector<PointLight> point_lights;
        /*point_lights.emplace_back(Point(-0.45, 0.94, 0), Vector3d(0.5, 0.5, 0.5));
        point_lights.emplace_back(Point(-0.35, 0.94, -0.1), Vector3d(0.5, 0.5, 0.5));
        point_lights.emplace_back(Point(0.15, 0.94, -0.2), Vector3d(0.5, 0.5, 0.5));
        point_lights.emplace_back(Point(0.35, 0.94, -0.3), Vector3d(0.5, 0.5, 0.5));
        point_lights.emplace_back(Point(0.45, 0.94, -0.4), Vector3d(0.5, 0.5, 0.5));*/
        return Scene(figures, point_lights, camera);
    }

    static Scene cornell_box_constructive_solid_geometry(size_t width, size_t height, size_t rays_per_pixel) {
        Point O(0, 0, -3.5);
        Direction U(0, 1, 0);
        Direction F(0, 0, 3);
        Camera camera(O, U, F, width, height, rays_per_pixel);
        Plane left_plane(1, Direction(1, 0, 0), Vector3d(0.8, 0, 0), Vector3d(0, 0, 0), Vector3d(0, 0, 0), 1.0);
        Plane right_plane(1, Direction(-1, 0, 0), Vector3d(0, 0.8, 0), Vector3d(0, 0, 0), Vector3d(0, 0, 0), 1.0);
        Plane floor_plane(1, Direction(0, 1, 0), Vector3d(0.6, 0.6, 0.6), Vector3d(0, 0, 0), Vector3d(0, 0, 0), 1.0);
        Plane ceiling_plane(1, Direction(0, -1, 0), Vector3d(1, 1, 1)); // Luz de área
        Plane back_plane(1, Direction(0, 0, -1), Vector3d(0.6, 0.6, 0.6), Vector3d(0, 0, 0), Vector3d(0, 0, 0), 1.0);
        Vector3d tr_diffuse_coefficient(0.3, 0.5, 0.7);
        Vector3d tr_refraction_coefficient(0, 0, 0);
        Vector3d tr_reflection_coefficient(1, 1, 1);
        double tr_refraction_index = 1;

        Disk disco(Point(-0.5,-0.5, 0.25), 0.3,
                   tr_diffuse_coefficient,
                   tr_refraction_coefficient,
                   tr_reflection_coefficient,
                   tr_refraction_index);
        Cylinder cilindro(Point(0,-0.4,0.35), 0.2, 0.7,
                          Vector3d(0.3, 0.5, 0.7),
                          tr_refraction_coefficient,
                          tr_reflection_coefficient,
                          tr_refraction_index);
        Cylinder cilindro2(Point(0,0,0.35), 0.2, 1.4,
                           Vector3d(0.3, 0.5, 0.7),
                           tr_refraction_coefficient,
                           tr_reflection_coefficient,
                           tr_refraction_index);
        TransformationMatrix m1 = TransformationMatrix::RotationMatrixOnY(M_PI/2);
        TransformationMatrix m2 = TransformationMatrix::RotationMatrixOnX(M_PI/2);
        TransformedFigure cilindroGirado1 = TransformedFigure(m2, std::make_shared<Cylinder>(cilindro),
                                                              Vector3d(0.3, 0.5, 0.7),
                                                              tr_refraction_coefficient,
                                                              tr_reflection_coefficient,
                                                              tr_refraction_index);
        TransformationMatrix m3 = TransformationMatrix::ScaleMatrix(1,2,1);
        TransformedFigure cilindroGirado1Grande = TransformedFigure(m3, std::make_shared<Cylinder>(cilindro),
                                                                    Vector3d(0.3, 0.5, 0.7),
                                                                    tr_refraction_coefficient,
                                                                    tr_reflection_coefficient,
                                                                    tr_refraction_index);

        ConstructiveSolidUnion unionCilindros(std::make_shared<TransformedFigure>(cilindroGirado1),std::make_shared<Cylinder>(cilindro2));

        Sphere circ(Point(0,0,0.35), 0.7,
                    Vector3d(0.3, 0.5, 0.7),
                    tr_refraction_coefficient,
                    tr_reflection_coefficient,
                    tr_refraction_index);

        Plane back_plane_aux(0.8, Direction(0, 0, -1),
                             Vector3d(249/255.0, 215/255.0, 28/255.0),
                             Vector3d(0, 0, 0), Vector3d(0, 0, 0), 1.0);


        ConstructiveSolidDifference plano_agujero(std::make_shared<Plane>(back_plane_aux), std::make_shared<Sphere>(circ));

        std::vector<std::shared_ptr<Figure>> figures;
        //figures.push_back(std::make_shared<Disk>(disco));
        //figures.push_back(std::make_shared<Disk>(disco));
        //figures.push_back(std::make_shared<Cylinder>(cilindro2));
        //figures.push_back(std::make_shared<ConstructiveSolidUnion>(unionCilindros));
        figures.push_back(std::make_shared<Disk>(disco));
        figures.push_back(std::make_shared<ConstructiveSolidDifference>(plano_agujero));
        figures.push_back(std::make_shared<ConstructiveSolidUnion>(unionCilindros));
        figures.push_back(std::make_shared<Plane>(left_plane));
        figures.push_back(std::make_shared<Plane>(right_plane));
        figures.push_back(std::make_shared<Plane>(floor_plane));
        figures.push_back(std::make_shared<Plane>(ceiling_plane));
        figures.push_back(std::make_shared<Plane>(back_plane));
        // Una luz puntual debajo de la luz de área
        std::vector<PointLight> point_lights;
        //point_lights.emplace_back(Point(0, 0.5, 0), Vector3d(1, 1, 1));
        return Scene(figures, point_lights, camera);
    }


    static Scene stanford_dragon_untextured(size_t width, size_t height, size_t rays_per_pixel) {
        Point O(0, 0, -3.5);
        Direction U(0, 1, 0);
        Direction F(0, 0, 3);

        Camera camera(O, U, F, width, height, rays_per_pixel);

        Plane left_plane(1, Direction(1, 0, 0), Vector3d(0.8, 0, 0), Vector3d(0, 0, 0), Vector3d(0, 0, 0), 1.0);
        Plane right_plane(1, Direction(-1, 0, 0), Vector3d(0, 0.8, 0), Vector3d(0, 0, 0), Vector3d(0, 0, 0), 1.0);
        Plane floor_plane(1, Direction(0, 1, 0), Vector3d(0.6, 0.6, 0.6), Vector3d(0, 0, 0), Vector3d(0, 0, 0), 1.0);
        Plane ceiling_plane(1, Direction(0, -1, 0), Vector3d(1, 1, 1)); // Luz de área
        Plane back_plane(1, Direction(0, 0, -1), Vector3d(0.6, 0.6, 0.6), Vector3d(0, 0, 0), Vector3d(0, 0, 0), 1.0);

        Vector3d tr_diffuse_coefficient = Vector3d(220 / 255.0, 220 / 255.0, 220 / 255.0); // "Marble"
        Vector3d tr_refraction_coefficient(0.06, 0.06, 0.06);
        Vector3d tr_reflection_coefficient(0.06, 0.06, 0.06);
        double tr_refraction_index = 1.5;
        std::vector<std::shared_ptr<Figure>> figures;
        auto triangulos = load_obj("objs/modelos_finales/dragon.obj", tr_diffuse_coefficient, tr_refraction_coefficient,
                                   tr_reflection_coefficient, tr_refraction_index);
        for (const auto& triangulo : triangulos)
            figures.push_back(std::make_shared<Triangle>(triangulo));

        figures.push_back(std::make_shared<Plane>(left_plane));
        figures.push_back(std::make_shared<Plane>(right_plane));
        figures.push_back(std::make_shared<Plane>(floor_plane));
        figures.push_back(std::make_shared<Plane>(ceiling_plane));
        figures.push_back(std::make_shared<Plane>(back_plane));

        // Una luz puntual debajo de la luz de área
        std::vector<PointLight> point_lights;
        //point_lights.emplace_back(PointLight(Point(0, 0.5, 0), Vector3d(1, 1, 1)));

        return Scene(figures, point_lights, camera);
    }

    static Scene stanford_dragon_pl(size_t width, size_t height, size_t rays_per_pixel) {
        Point O(0, 0, -3.5);
        Direction U(0, 1, 0);
        Direction F(0, 0, 3);

        Camera camera(O, U, F, width, height, rays_per_pixel);

        //std::shared_ptr<Texture> texture = std::make_shared<Texture>("objs/modelos_finales/dragon.ppm");

        Plane left_plane(1, Direction(1, 0, 0), Vector3d(0.8, 0, 0), Vector3d(0, 0, 0), Vector3d(0, 0, 0), 1.0);
        Plane right_plane(1, Direction(-1, 0, 0), Vector3d(0, 0.8, 0), Vector3d(0, 0, 0), Vector3d(0, 0, 0), 1.0);
        Plane floor_plane(1, Direction(0, 1, 0), Vector3d(0.6, 0.6, 0.6), Vector3d(0, 0, 0), Vector3d(0, 0, 0), 1.0);
        Plane ceiling_plane(1, Direction(0, -1, 0), Vector3d(0.6, 0.6, 0.6), Vector3d(0, 0, 0), Vector3d(0, 0, 0), 1.0);
        Plane back_plane(1, Direction(0, 0, -1), Vector3d(0.6, 0.6, 0.6), Vector3d(0, 0, 0), Vector3d(0, 0, 0), 1.0);

        double tr_refraction_index = 1.5;
        std::vector<std::shared_ptr<Figure>> figures;
        auto triangulos = load_obj("objs/modelos_finales/dragon.obj",
                                   Vector3d(0, 0, 0),
                                   Vector3d(0.85, 0.85, 0.85),
                                   Vector3d(0.1, 0.1, 0.1),
                                   tr_refraction_index);
        for (const auto& triangulo : triangulos)
            figures.push_back(std::make_shared<Triangle>(triangulo));

        figures.push_back(std::make_shared<Plane>(left_plane));
        figures.push_back(std::make_shared<Plane>(right_plane));
        figures.push_back(std::make_shared<Plane>(floor_plane));
        figures.push_back(std::make_shared<Plane>(ceiling_plane));
        figures.push_back(std::make_shared<Plane>(back_plane));

        std::vector<PointLight> point_lights;
        point_lights.emplace_back(PointLight(Point(0.7, 0.7, 0), Vector3d(1, 1, 1)));

        return Scene(figures, point_lights, camera);
    }


    static Scene stanford_dragon_textured(size_t width, size_t height, size_t rays_per_pixel) {
        Point O(0, 0, -3.5);
        Direction U(0, 1, 0);
        Direction F(0, 0, 3);

        Camera camera(O, U, F, width, height, rays_per_pixel);

        std::shared_ptr<Texture> texture = std::make_shared<Texture>("objs/modelos_finales/dragon.ppm");

        Plane left_plane(1, Direction(1, 0, 0), Vector3d(0.8, 0, 0), Vector3d(0, 0, 0), Vector3d(0, 0, 0), 1.0);
        Plane right_plane(1, Direction(-1, 0, 0), Vector3d(0, 0.8, 0), Vector3d(0, 0, 0), Vector3d(0, 0, 0), 1.0);
        Plane floor_plane(1, Direction(0, 1, 0), Vector3d(0.6, 0.6, 0.6), Vector3d(0, 0, 0), Vector3d(0, 0, 0), 1.0);
        Plane ceiling_plane(1, Direction(0, -1, 0), Vector3d(1, 1, 1)); // Luz de área
        Plane back_plane(1, Direction(0, 0, -1), Vector3d(0.6, 0.6, 0.6), Vector3d(0, 0, 0), Vector3d(0, 0, 0), 1.0);

        Vector3d tr_diffuse_coefficient = Vector3d(220 / 255.0, 220 / 255.0, 220 / 255.0); // "Marble"
        Vector3d tr_refraction_coefficient(0.03, 0.03, 0.03);
        Vector3d tr_reflection_coefficient(0.03, 0.03, 0.03);
        double tr_refraction_index = 1.5;
        std::vector<std::shared_ptr<Figure>> figures;
        auto triangulos = load_obj("objs/modelos_finales/dragon.obj", tr_diffuse_coefficient, tr_refraction_coefficient,
                                   tr_reflection_coefficient, tr_refraction_index, texture);
        for (const auto& triangulo : triangulos)
            figures.push_back(std::make_shared<Triangle>(triangulo));

        figures.push_back(std::make_shared<Plane>(left_plane));
        figures.push_back(std::make_shared<Plane>(right_plane));
        figures.push_back(std::make_shared<Plane>(floor_plane));
        figures.push_back(std::make_shared<Plane>(ceiling_plane));
        figures.push_back(std::make_shared<Plane>(back_plane));

        // Una luz puntual debajo de la luz de área
        std::vector<PointLight> point_lights;
        //point_lights.emplace_back(PointLight(Point(0, 0.5, 0), Vector3d(1, 1, 1)));

        return Scene(figures, point_lights, camera);
    }


    static Scene camera_demo(size_t width, size_t height, size_t rays_per_pixel) {
        Point O(0.8, 0.55, -3.5); // Un poco a la derecha, mirando desde arriba

        // Equivalentes en 16:9
        //Camera camera(O, Point(Vector3d(0, 0, -0.5)), width, height, rays_per_pixel, 36.86);
        Camera camera(O, Point(Vector3d(0, -0.5, -0.5)), width, height, rays_per_pixel); // Hacia el centro, un poco hacia abajo

        std::shared_ptr<Texture> chevron_texture = std::make_shared<Texture>("texture_files/chevron_3.ppm");

        Plane left_plane(1, Direction(1, 0, 0), Vector3d(0.8, 0, 0), Vector3d(0, 0, 0), Vector3d(0, 0, 0), 1.0);
        Plane right_plane(1, Direction(-1, 0, 0), Vector3d(0, 0.8, 0), Vector3d(0, 0, 0), Vector3d(0, 0, 0), 1.0);
        Plane floor_plane(1, Direction(0, 1, 0), Vector3d(0.6, 0.6, 0.6), Vector3d(0, 0, 0), Vector3d(0, 0, 0), 1.0, chevron_texture);
        Plane ceiling_plane(1, Direction(0, -1, 0), Vector3d(1, 1, 1)); // Luz de área
        Plane back_plane(1, Direction(0, 0, -1), Vector3d(0.6, 0.6, 0.6), Vector3d(0, 0, 0), Vector3d(0, 0, 0), 1.0);

        // dif + spec
        Sphere s_left(Point(-0.5,-0.7, 0.25), 0.3, Vector3d(0.3, 0.5, 0.7), Vector3d(0, 0, 0), Vector3d(0.25, 0.25, 0.25), 1.0);

        // refr + spec
        Sphere s_right(Point(0.5,-0.7,-0.25), 0.3, Vector3d(0, 0, 0), Vector3d(0.85, 0.85, 0.85), Vector3d(0.1, 0.1, 0.1), 1.5); // Cristal

        std::vector<std::shared_ptr<Figure>> figures;
        figures.push_back(std::make_shared<Sphere>(s_right));
        figures.push_back(std::make_shared<Sphere>(s_left));
        figures.push_back(std::make_shared<Plane>(left_plane));
        figures.push_back(std::make_shared<Plane>(right_plane));
        figures.push_back(std::make_shared<Plane>(floor_plane));
        figures.push_back(std::make_shared<Plane>(ceiling_plane));
        figures.push_back(std::make_shared<Plane>(back_plane));

        // Una luz puntual debajo de la luz de área
        std::vector<PointLight> point_lights;
        //point_lights.emplace_back(Point(0, 0.5, 0), Vector3d(1, 1, 1));

        return Scene(figures, point_lights, camera);
    }


    static Scene camera_demo_fov(size_t width, size_t height, size_t rays_per_pixel) {
        Point O(0.8, 0.55, -3.5); // Un poco a la derecha, mirando desde arriba

        // Equivalentes en 16:9
        //Camera camera(O, Point(Vector3d(0, 0, -0.5)), width, height, rays_per_pixel, 36.86);
        Camera camera(O, Point(Vector3d(0, -0.5, -0.5)), width, height, rays_per_pixel, 65); // Hacia el centro, un poco hacia abajo

        std::shared_ptr<Texture> chevron_texture = std::make_shared<Texture>("texture_files/chevron_3.ppm");

        Plane left_plane(1, Direction(1, 0, 0), Vector3d(0.8, 0, 0), Vector3d(0, 0, 0), Vector3d(0, 0, 0), 1.0);
        Plane right_plane(1, Direction(-1, 0, 0), Vector3d(0, 0.8, 0), Vector3d(0, 0, 0), Vector3d(0, 0, 0), 1.0);
        Plane floor_plane(1, Direction(0, 1, 0), Vector3d(0.6, 0.6, 0.6), Vector3d(0, 0, 0), Vector3d(0, 0, 0), 1.0, chevron_texture);
        Plane ceiling_plane(1, Direction(0, -1, 0), Vector3d(1, 1, 1)); // Luz de área
        Plane back_plane(1, Direction(0, 0, -1), Vector3d(0.6, 0.6, 0.6), Vector3d(0, 0, 0), Vector3d(0, 0, 0), 1.0);

        // dif + spec
        Sphere s_left(Point(-0.5,-0.7, 0.25), 0.3, Vector3d(0.3, 0.5, 0.7), Vector3d(0, 0, 0), Vector3d(0.25, 0.25, 0.25), 1.0);

        // refr + spec
        Sphere s_right(Point(0.5,-0.7,-0.25), 0.3, Vector3d(0, 0, 0), Vector3d(0.85, 0.85, 0.85), Vector3d(0.1, 0.1, 0.1), 1.5); // Cristal

        std::vector<std::shared_ptr<Figure>> figures;
        figures.push_back(std::make_shared<Sphere>(s_right));
        figures.push_back(std::make_shared<Sphere>(s_left));
        figures.push_back(std::make_shared<Plane>(left_plane));
        figures.push_back(std::make_shared<Plane>(right_plane));
        figures.push_back(std::make_shared<Plane>(floor_plane));
        figures.push_back(std::make_shared<Plane>(ceiling_plane));
        figures.push_back(std::make_shared<Plane>(back_plane));

        // Una luz puntual debajo de la luz de área
        std::vector<PointLight> point_lights;
        //point_lights.emplace_back(Point(0, 0.5, 0), Vector3d(1, 1, 1));

        return Scene(figures, point_lights, camera);
    }


    static Scene plane_textures(size_t width, size_t height, size_t rays_per_pixel) {
        Point O(0, 0, -3.5);
        Direction U(0, 1, 0);
        Direction F(0, 0, 3);

        Camera camera(O, U, F, width, height, rays_per_pixel);

        std::shared_ptr<Texture> texture = std::make_shared<Texture>("objs/modelos_finales/dragon.ppm");

        Plane left_plane(1, Direction(1, 0, 0), Vector3d(0.8, 0, 0), Vector3d(0, 0, 0), Vector3d(0, 0, 0), 1.0);
        Plane right_plane(1, Direction(-1, 0, 0), Vector3d(0, 0.8, 0), Vector3d(0, 0, 0), Vector3d(0, 0, 0), 1.0);
        Plane floor_plane(1, Direction(0, 1, 0), Vector3d(0.6, 0.6, 0.6), Vector3d(0, 0, 0), Vector3d(0, 0, 0), 1.0);
        Plane ceiling_plane(1, Direction(0, -1, 0), Vector3d(1, 1, 1)); // Luz de área
        Plane back_plane(1, Direction(0, 0, -1), Vector3d(0.6, 0.6, 0.6), Vector3d(0, 0, 0), Vector3d(0, 0, 0), 1.0);

        Vector3d tr_diffuse_coefficient = Vector3d(220 / 255.0, 220 / 255.0, 220 / 255.0); // "Marble"
        Vector3d tr_refraction_coefficient(0.03, 0.03, 0.03);
        Vector3d tr_reflection_coefficient(0.03, 0.03, 0.03);
        double tr_refraction_index = 1.5;
        std::vector<std::shared_ptr<Figure>> figures;
        auto triangulos = load_obj("objs/modelos_finales/dragon.obj", tr_diffuse_coefficient, tr_refraction_coefficient,
                                   tr_reflection_coefficient, tr_refraction_index, texture);
        for (const auto& triangulo : triangulos)
            figures.push_back(std::make_shared<Triangle>(triangulo));

        figures.push_back(std::make_shared<Plane>(left_plane));
        figures.push_back(std::make_shared<Plane>(right_plane));
        figures.push_back(std::make_shared<Plane>(floor_plane));
        figures.push_back(std::make_shared<Plane>(ceiling_plane));
        figures.push_back(std::make_shared<Plane>(back_plane));

        // Una luz puntual debajo de la luz de área
        std::vector<PointLight> point_lights;
        //point_lights.emplace_back(PointLight(Point(0, 0.5, 0), Vector3d(1, 1, 1)));

        return Scene(figures, point_lights, camera);
    }
};

#endif //INFORMATICA_GRAFICA_SCENE_HPP
