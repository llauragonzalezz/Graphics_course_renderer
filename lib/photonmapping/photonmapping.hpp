//
// photonmapping.hpp
//
// Description:
//  Photonmapping algorithm implementation
//
// Authors:
//  Samuel García
//  Laura González
//
// Date:
//  11/2022.
//

#ifndef INFORMATICA_GRAFICA_PHOTONMAPPING_HPP
#define INFORMATICA_GRAFICA_PHOTONMAPPING_HPP

#include <random>
#include "../Scene.hpp"
#include "pathtracing.hpp"
#include "../Photon.hpp"
#include "photonmapping_kdtree.hpp"


enum PhotonmappingKernel {
    BOX,
    NORMALIZED_BOX,
    GAUSSIAN,
    NORMALIZED_GAUSSIAN,
    CONE
};

enum PhotonmappingDirectLightMethod {
    NEXT_EVENT_ESTIMATION,
    STORE_ALL_PHOTONS
};

class PhotonMapperRayGenerator {
    std::deque<std::pair<PointLight, size_t>> point_lights;

public:
    PhotonMapperRayGenerator(const std::vector<PointLight>& _point_lights, size_t walks) {
        double sum_of_powers = 0;
        for (auto pl : _point_lights) {
            sum_of_powers += std::max(std::max(pl.power[0], pl.power[1]), pl.power[2]);
        }

        for (auto pl : _point_lights) {
            double p = std::max(std::max(pl.power[0], pl.power[1]), pl.power[2]);
            point_lights.emplace_back(std::make_pair<>(pl, (double) walks * (p / sum_of_powers)));
        }
    }

    // Create a ray towards any of the point lights. If none is generated (we reached the limit), returns an invalid
    // ray and false
    std::pair<Ray, bool> generate_ray() {
        if (point_lights.empty()) return std::make_pair<>(Ray(), false);

        static std::mt19937 gen = std::mt19937((std::random_device()()));
        static std::uniform_real_distribution<double> phi_distr = std::uniform_real_distribution<double>(0, 1.0);
        static std::uniform_real_distribution<double> thita_distr = std::uniform_real_distribution<double>(0.0, 1.0);

        double eThita = thita_distr(gen);
        double ePhi = phi_distr(gen);

        double thita = std::acos(2*eThita-1); // Random [0, pi) value
        double phi = 2*M_PI*ePhi; // Random [0, 2pi) value

        // SPherical to cartesian coordinates conversion
        Ray new_ray = Ray(point_lights[0].first.center,
                          Direction(Vector3d(sin(thita)*cos(phi),
                                             sin(thita)*sin(phi),
                                             cos(thita))),
                          point_lights[0].first.power);

        point_lights[0].second--;
        if (point_lights[0].second == 0)  point_lights.pop_front();

        return {std::make_pair(new_ray, true)};
    }
};

void scatter_photons(const Scene &scene,
                     const Ray ray,
                     const size_t number_of_bounces,
                     std::vector<Photon> &photons,
                     Vector3d throughput, // Accumulated BRDF
                     PhotonmappingDirectLightMethod method) {
    if (number_of_bounces > (MAX_NUM_OF_BOUNCES)) return;
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

    if (!reg.hits) return;

    if (reg.is_area_light) {
        return;
    } else {
        Event event = russian_roulette(reg);
        Ray w_i = generate_wi(event, reg, ray);
        w_i.flux = ray.flux;

        if (event == DIFFUSE && (method == STORE_ALL_PHOTONS || number_of_bounces > 0)) {
            photons.emplace_back(reg.n.origin, ray.direction.v, ray.flux.element_by_element(throughput));
        }

        if (event != ABSORPTION) {
             scatter_photons(scene,
                             w_i,
                             number_of_bounces + 1,
                             photons,
                             throughput.element_by_element(material_properties(event, reg)),
                             method);
        }
    }
}

void scatter_photons_bvh(const Scene &scene,
                     const BVH &bvh_tree,
                     const Ray ray,
                     const size_t number_of_bounces,
                     std::vector<Photon> &photons,
                     Vector3d throughput, // Accumulated BRDF
                     PhotonmappingDirectLightMethod method) {
    if (number_of_bounces > (MAX_NUM_OF_BOUNCES)) return;
#ifdef benchmarking
    Benchmarking::count_ray_traced();
#endif
    HitRegister reg = bvh_tree.collides(ray);

    if (!reg.hits) return;

    if (reg.is_area_light) {
        return;
    } else {
        Event event = russian_roulette(reg);
        Ray w_i = generate_wi(event, reg, ray);
        w_i.flux = ray.flux;

        if (event == DIFFUSE && (method == STORE_ALL_PHOTONS || number_of_bounces > 0)) {
            photons.emplace_back(reg.n.origin, ray.direction.v, ray.flux.element_by_element(throughput));
        }

        if (event != ABSORPTION) {
             scatter_photons_bvh(scene,
                             bvh_tree,
                             w_i,
                             number_of_bounces + 1,
                             photons,
                             throughput.element_by_element(material_properties(event, reg)),
                             method);
        }
    }
}

// Diffuse BSDF evaluation
Vector3d integrator_sample_photonmapping(const Scene &scene,
                                         const nn::KDTree<Photon, 3, PhotonAxisPosition> &photons,
                                         const Ray ray,
                                         PhotonmappingKernel kernel,
                                         PhotonmappingDirectLightMethod method) {
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

        if (event == DIFFUSE) {
            Vector3d direct_light_contributions = get_contributions_from_direct_lights(scene, reg);

            size_t number_of_photons = 25;            // Maximum number of photons to look for
            //double radius_estimate = 0.1;           // Maximum distance to look for photons
            auto nearest_neighbors = photons.nearest_neighbors(reg.n.origin.v,
                                                               number_of_photons,
                                                               std::numeric_limits<float>::max());

            // Adjust the radius to the farthest photon
            double max_radio = 0;
            for (auto photon : nearest_neighbors){
                max_radio = std::max(max_radio, (reg.n.origin.v - photon->position.v).modulus());
            }

            Vector3d flux_sum;
            double d;
            switch (kernel) {
                case BOX:
                    for (auto photon : nearest_neighbors) flux_sum = flux_sum + photon->flux;

                    break;

                case NORMALIZED_BOX:
                    for (auto photon : nearest_neighbors) flux_sum = flux_sum + photon->flux;
                    flux_sum = flux_sum / (M_PI * max_radio * max_radio);

                    break;

                case GAUSSIAN:
                    // Gaussian kernel based on:
                    //  https://www.researchgate.net/publication/261793571_Overestimation_and_Underestimation_Biases_in_Photon_Mapping_with_Non-Constant_Kernels
                    for (auto photon : nearest_neighbors) {
                        d = (reg.n.origin.v - photon->position.v).modulus();
                        flux_sum = flux_sum + photon->flux*1.728*(1-(1-exp((-1.953*d*d)/(2*max_radio*max_radio)))/(1-exp(-1.953))); // Alpha = 1.728, beta = 1.953
                    }

                    break;

                case NORMALIZED_GAUSSIAN:
                    for (auto photon : nearest_neighbors) {
                        d = (reg.n.origin.v - photon->position.v).modulus();
                        flux_sum = flux_sum + photon->flux*1.728*(1-(1-exp((-1.953*d*d)/(2*max_radio*max_radio)))/(1-exp(-1.953))); // Alpha = 1.728, beta = 1.953
                    }

                    flux_sum = flux_sum / (M_PI * max_radio * max_radio);

                    break;

                case CONE:
                    for (auto photon : nearest_neighbors) {
                        d = (reg.n.origin.v - photon->position.v).modulus();
                        flux_sum = flux_sum + photon->flux * (1 - d/(1.1*max_radio)); // Alpha = 1.728, beta = 1.953
                    }

                    flux_sum = flux_sum / ((1 - 2/(3*1.1))* M_PI * max_radio * max_radio);

                    break;
            }

            flux_sum = flux_sum.element_by_element(reg.diffuse_coefficient/M_PI);

            if (method == NEXT_EVENT_ESTIMATION) flux_sum = flux_sum + direct_light_contributions;
            return flux_sum;
        } else if (event != ABSORPTION) {
            return integrator_sample_photonmapping(scene, photons, w_i, kernel, method);
        } else {
            return {0, 0, 0};
        }
    }
}

Vector3d integrator_sample_photonmapping_bvh(const Scene &scene,
                                             const BVH &bvh_tree,
                                             const nn::KDTree<Photon, 3, PhotonAxisPosition> &photons,
                                             const Ray ray,
                                             PhotonmappingKernel kernel,
                                             PhotonmappingDirectLightMethod method) {
#ifdef benchmarking
    Benchmarking::count_ray_traced();
#endif
    HitRegister reg = bvh_tree.collides(ray);

    if (!reg.hits) {
        return {0, 0, 0};
    }

    if (reg.is_area_light) {
        return reg.emission;
    } else {
        Event event = russian_roulette(reg);
        Ray w_i = generate_wi(event, reg, ray);

        if (event == DIFFUSE) {
            Vector3d direct_light_contributions = get_contributions_from_direct_lights_bvh(scene, bvh_tree, reg);

            size_t number_of_photons = 25;           // Maximum number of photons to look for
            //double radius_estimate = 0.1;           // Maximum distance to look for photons
            auto nearest_neighbors = photons.nearest_neighbors(reg.n.origin.v,
                                                               number_of_photons,
                                                               std::numeric_limits<float>::max());

            double max_radio = 0;
            for (auto photon : nearest_neighbors){
                max_radio = std::max(max_radio, (reg.n.origin.v - photon->position.v).modulus());
            }

            Vector3d flux_sum;
            double d;
            switch (kernel) {
                case BOX:
                    for (auto photon : nearest_neighbors) flux_sum = flux_sum + photon->flux;

                    break;

                case NORMALIZED_BOX:
                    for (auto photon : nearest_neighbors) flux_sum = flux_sum + photon->flux;
                    flux_sum = flux_sum / (M_PI * max_radio * max_radio);

                    break;

                case GAUSSIAN:
                    // Gaussian kernel based on:
                    //      https://www.researchgate.net/publication/261793571_Overestimation_and_Underestimation_Biases_in_Photon_Mapping_with_Non-Constant_Kernels
                    for (auto photon : nearest_neighbors) {
                        d = (reg.n.origin.v - photon->position.v).modulus();
                        flux_sum = flux_sum + photon->flux*1.728*(1-(1-exp((-1.953*d*d)/(2*max_radio*max_radio)))/(1-exp(-1.953))); // Alpha = 1.728, beta = 1.953
                    }

                    break;

                case NORMALIZED_GAUSSIAN:
                    for (auto photon : nearest_neighbors) {
                        d = (reg.n.origin.v - photon->position.v).modulus();
                        flux_sum = flux_sum + photon->flux*1.728*(1-(1-exp((-1.953*d*d)/(2*max_radio*max_radio)))/(1-exp(-1.953))); // Alpha = 1.728, beta = 1.953
                    }

                    flux_sum = flux_sum / (M_PI * max_radio * max_radio);

                    break;

                case CONE:
                    for (auto photon : nearest_neighbors) {
                        d = (reg.n.origin.v - photon->position.v).modulus();
                        flux_sum = flux_sum + photon->flux * (1 - d/(1.1*max_radio)); // Alpha = 1.728, beta = 1.953
                    }

                    flux_sum = flux_sum / ((1 - 2/(3*1.1))* M_PI * max_radio * max_radio);

                    break;
            }

            flux_sum = flux_sum.element_by_element(reg.diffuse_coefficient/M_PI);

            if (method == NEXT_EVENT_ESTIMATION) flux_sum = flux_sum + direct_light_contributions;
            return flux_sum;
        } else if (event != ABSORPTION) {
            return integrator_sample_photonmapping_bvh(scene, bvh_tree, photons, w_i, kernel, method);
        } else {
            return {0, 0, 0};
        }
    }
}


#endif //INFORMATICA_GRAFICA_PHOTONMAPPING_HPP
