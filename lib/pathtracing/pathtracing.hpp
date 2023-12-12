//
// pathtracing.hpp
//
// Description:
//  Pathtracing algorithm implementation
//
// Authors:
//  Samuel García
//  Laura González
//
// Date:
//  11/2022.
//

#ifndef INFORMATICA_GRAFICA_PATHTRACING_HPP
#define INFORMATICA_GRAFICA_PATHTRACING_HPP

#include "../lib/math/Vector3d.hpp"
#include "HitRegister.hpp"
#include "../lib/PointLight.hpp"
#include "../lib/Scene.hpp"
#include "../math/TransformationMatrix.hpp"
#include "BVH.hpp"

#ifdef benchmarking
#include "benchmarking.hpp"
#endif

#define MAX_NUM_OF_BOUNCES 6
#define AIR_REFRACTION 1.00027717

enum Event {
    DIFFUSE = 0,
    SPECULAR = 1,
    REFRACTION = 2,
    ABSORPTION = 3
};


Event russian_roulette(const HitRegister &reg) {
    static std::mt19937 gen = std::mt19937(std::random_device()());
    static std::uniform_real_distribution<double> distr = std::uniform_real_distribution<double>(0, 1.0);

    double p_d = std::max(std::max(reg.diffuse_coefficient[0], reg.diffuse_coefficient[1]), reg.diffuse_coefficient[2]);
    double p_s = std::max(std::max(reg.reflection_coefficient[0], reg.reflection_coefficient[1]), reg.reflection_coefficient[2]);
    double p_t = std::max(std::max(reg.refraction_coefficient[0], reg.refraction_coefficient[1]), reg.refraction_coefficient[2]);
    double num = distr(gen);

    if (num <= p_d) return DIFFUSE;
    else if (num > p_d && num <= p_d + p_s) return SPECULAR;
    else if (num > p_d + p_s && num <= p_d + p_s + p_t) return REFRACTION;
    else return ABSORPTION;
}


// Generate a random ray on the hemisphere of a hit point
Ray brdf_sample(const HitRegister &reg) {
    static std::mt19937 gen = std::mt19937((std::random_device()()));
    static std::uniform_real_distribution<double> bsdf_distr = std::uniform_real_distribution<double>(0, 1.0);

    double eThita = bsdf_distr(gen);
    double ePhi = bsdf_distr(gen);

    double thita = std::acos(std::sqrt(1-eThita)); // Random [0, pi/2) value
    double phi = 2*M_PI*ePhi; // Random [0, 2pi) value

    // Spherical to cartesian coordinates conversion
    return {reg.n.origin, Direction(Vector3d(sin(thita)*cos(phi),
                                             sin(thita)*sin(phi),
                                             cos(thita)))};
}


Ray generate_wi(Event event, const HitRegister &reg, const Ray &w_o) {
    switch (event) {
        case ABSORPTION: // Should never happend
            return {};

        case DIFFUSE:
            return brdf_sample(reg);

        case SPECULAR:
            return {Point(reg.n.origin), Direction(w_o.direction.v - 2 * (reg.n.direction.v * (w_o.direction.v.dot(reg.n.direction.v))))};

        // Adapted from:
        //  https://www.scratchapixel.com/lessons/3d-basic-rendering/introduction-to-shading/reflection-refraction-fresnel
        case REFRACTION:
            Vector3d Nrefr = reg.n.direction.v;
            double cosi = std::clamp(-1.0, 1.0, (w_o.direction.v).dot(Nrefr));
            double NdotI = Nrefr.dot(w_o.direction.v);
            double etai = AIR_REFRACTION, etat = reg.refraction_index;
            if (NdotI < 0) {
                // We are outside the figure's surface, force cos(theta) to be positive
                NdotI = -NdotI;
            }
            else {
                // We are inside the figure's surface, cos(theta) is positive but
                // we must force the refraction's direction to be that of the surface
                // normal's, negated
                Nrefr = (-1.0) * reg.n.direction.v;

                // Swap the refraction coefficients
                std::swap(etai, etat);
            }

            double eta = etai / etat;
            double k = 1 - eta * eta * (1 - cosi * cosi);

            if (k < 0) {
                // Total internal reflection, force to be no refraction
                return {Point(reg.n.origin), Vector3d(0, 0, 0)};
            } else {
                // Create the refraction's direction
                Direction new_direction = Direction(eta * (w_o.direction.v) + (eta * NdotI - sqrt(k)) * Nrefr);
                // Advance a bit across the direction in order to avoid colliding with the surface again (start a bit
                // further from it)
                return {Point(reg.n.origin.v + (0.01)*new_direction.v), new_direction};
            }
    }

    return {};
}


Vector3d material_properties(Event event, const HitRegister &reg) {
    double p;
    switch (event) {
        case DIFFUSE:
            p = std::max(std::max(reg.diffuse_coefficient[0], reg.diffuse_coefficient[1]),
                         reg.diffuse_coefficient[2]);
            return reg.diffuse_coefficient / p;
        case SPECULAR:
            p = std::max(std::max(reg.reflection_coefficient[0], reg.reflection_coefficient[1]),
                         reg.reflection_coefficient[2]);
            return (reg.reflection_coefficient / p);

        case REFRACTION:
            p = std::max(std::max(reg.refraction_coefficient[0], reg.refraction_coefficient[1]),
                         reg.refraction_coefficient[2]);
            return (reg.refraction_coefficient / p);

        case ABSORPTION:
        default:
            return {0, 0, 0};
    }
}


Vector3d get_contributions_from_direct_lights(const Scene &scene, const HitRegister &reg) {
    Vector3d emission;

    // We only look for point lights
    for (PointLight pl : scene.point_lights) {
        auto dir_hit_to_light = Direction((pl.center.v - reg.n.origin.v));

        Ray ray_to_light = Ray(reg.n.origin, dir_hit_to_light);

        bool hits_in_path = false;
        // Hit across the scene again, testing for collisions before reaching the point light
        for (const auto& figure : scene.figures) {
            HitRegister temp = figure->collides(ray_to_light);

            if (temp.hits && less_than(temp.t, (pl.center.v - reg.n.origin.v).modulus())) {
                hits_in_path = true;
                break;
            }
        }

        if (!hits_in_path) {
            Vector3d radiance = pl.power / (std::pow(dir_hit_to_light.v.modulus(), 2));
            Vector3d diffuse_material_properties = reg.diffuse_coefficient / M_PI;
            double cosine_term = reg.n.direction.v.dot(dir_hit_to_light.v);


            emission = emission + Vector3d(radiance[0] * diffuse_material_properties[0] * cosine_term,
                                           radiance[1] * diffuse_material_properties[1] * cosine_term,
                                           radiance[2] * diffuse_material_properties[2] * cosine_term);
        }
    }

    return emission;
}


// Same implementation as above, using a BVH tree
Vector3d get_contributions_from_direct_lights_bvh(const Scene &scene, const BVH &bvh_tree, const HitRegister &reg) {
    Vector3d emission;

    // We only look for point lights
    for (PointLight pl : scene.point_lights) {
        auto dir_hit_to_light = Direction((pl.center.v - reg.n.origin.v));

        Ray ray_to_light = Ray(reg.n.origin, dir_hit_to_light);

        HitRegister temp = bvh_tree.collides(ray_to_light);
        bool hits_in_path = temp.hits && less_than(temp.t, (pl.center.v - reg.n.origin.v).modulus());

        if (!hits_in_path) {
            Vector3d radiance = pl.power / (std::pow(dir_hit_to_light.v.modulus(), 2));
            Vector3d diffuse_material_properties = reg.diffuse_coefficient / M_PI;
            double cosine_term = reg.n.direction.v.dot(dir_hit_to_light.v);


            emission = emission + Vector3d(radiance[0] * diffuse_material_properties[0] * cosine_term,
                                           radiance[1] * diffuse_material_properties[1] * cosine_term,
                                           radiance[2] * diffuse_material_properties[2] * cosine_term);
        }
    }

    return emission;
}

Vector3d integrator_sample_bvh(const Scene &scene, const BVH &bvh_tree, const Ray ray, const size_t number_of_bounces) {
    if (number_of_bounces > MAX_NUM_OF_BOUNCES) return {0,0,0};
#ifdef benchmarking
    Benchmarking::count_ray_traced();
#endif
    HitRegister reg = bvh_tree.collides(ray);

    if (!reg.hits) {
        return {0, 0, 0};
    }

    if (reg.is_area_light) {
        // Return the area light's emission
        return reg.emission;
    } else {
        Event event = russian_roulette(reg);
        Ray w_i = generate_wi(event, reg, ray);
        Vector3d mat_props = material_properties(event, reg);

        // Next-event estimation
        Vector3d direct_light_contributions;
        if (event != SPECULAR) direct_light_contributions = get_contributions_from_direct_lights_bvh(scene, bvh_tree, reg);
        else direct_light_contributions = Vector3d(0, 0, 0);

        return direct_light_contributions + (mat_props).element_by_element(integrator_sample_bvh(scene, bvh_tree, w_i, number_of_bounces + 1));
    }
}


// Diffuse BSDF evaluation
Vector3d integrator_sample(const Scene &scene, const Ray ray, const size_t number_of_bounces) {
    if (number_of_bounces > MAX_NUM_OF_BOUNCES) return {0,0,0};
#ifdef benchmarking
    Benchmarking::count_ray_traced();
#endif
    HitRegister reg, temp;
    reg.t = std::numeric_limits<double>::max();

    for (const auto& figure : scene.figures) {
        temp = figure->collides(ray);

        if (temp.hits  && less_than(temp.t, reg.t)) {
            reg = temp;
        }
    }

    if (!reg.hits) {
        return {0, 0, 0};
    }

    if (reg.is_area_light) {
        // Return the area light's emission
        return reg.emission;
    } else {
        Event event = russian_roulette(reg);
        Ray w_i = generate_wi(event, reg, ray);
        Vector3d mat_props = material_properties(event, reg);

        // Next-event estimation
        Vector3d direct_light_contributions;
        if (event != SPECULAR) direct_light_contributions = get_contributions_from_direct_lights(scene, reg);
        else direct_light_contributions = Vector3d(0, 0, 0);

        return direct_light_contributions + (mat_props).element_by_element(integrator_sample(scene, w_i, number_of_bounces + 1));
    }
}



#endif //INFORMATICA_GRAFICA_PATHTRACING_HPP
