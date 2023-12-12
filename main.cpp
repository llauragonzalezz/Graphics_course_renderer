#include <random>

#include "lib/Scene.hpp"
#include "pathtracer/multithreaded_pathtracer.hpp"
#include "pathtracer/multithreaded_pathtracer_bvh.hpp"
#include "photonmapper/multithreaded_photonmapper.hpp"
#include "photonmapper/multithreaded_photonmapper_bvh.hpp"

using namespace std;

int main() {
#ifdef benchmarking
    std::cerr << "Starting in Benchmarking mode, rendering time will be substantially higher..." << std::endl;
#endif
    size_t width = 1360;
    size_t height = 768;
    size_t rays_per_pixel = 64;
    /************************************************************************************************************************
     * If you want to tweak more internal parameters:                                                                       *
     *  Pathtracer:                                                                                                         *
     *   - Maximum number of bounces: MAX_NUM_OF_BOUNCES in pathtracing.hpp                                                 *
     *     (this doesn't prevent russian roulette absorption events from firing earlier)                                    *
     *                                                                                                                      *
     *  Photonmapper (can be tweaked for non-BVH and BVH rendering separately):                                             *
     *   - Maximum number of walks: MAX_WALKS in multithreaded_photonmapper.hpp and multithreaded_photonmapper_bvh.hpp      *
     *                                                                                                                      *
     *   - Maximum number of photons: MAX_PHOTONS in multithreaded_photonmapper.hpp and multithreaded_photonmapper_bvh.hpp  *
     *                                                                                                                      *
     *   - Number of photons to take from the map: number_of_photons variable in the integrator_sample_photonmapping        *
     *     and integrator_sample_photonmapping_bvh functions, in photonmapping.hpp                                          *
     *                                                                                                                      *
     ************************************************************************************************************************/


    /**************************************
     * Final demo scenes, uncomment       *
     * the one you want to render. Only   *
     * the ones with point lights will    *
     * be able to run on the photonmapper *
     **************************************/

    // Cornell boxes
    //Scene scene = Scene::cornell_box_area_light(width, height, rays_per_pixel);
    //Scene scene = Scene::cornell_box_two_point_lights(width, height, rays_per_pixel);
    //Scene scene = Scene::cornell_box_many_point_lights(width, height, rays_per_pixel);
    Scene scene = Scene::cornell_box_point_light(width, height, rays_per_pixel);
    //Scene scene = Scene::cornell_box_point_light_diffuse_spheres(width, height, rays_per_pixel);
    //Scene scene = Scene::cornell_box_diffuse_sphere(width, height, rays_per_pixel);
    //Scene scene = Scene::cornell_box_reflective_sphere(width, height, rays_per_pixel);
    //Scene scene = Scene::cornell_box_refractive_sphere(width, height, rays_per_pixel);
    //Scene scene = Scene::cornell_box_diffuse_sphere_point_light(width, height, rays_per_pixel);
    //Scene scene = Scene::cornell_box_reflective_sphere_point_light(width, height, rays_per_pixel);
    //Scene scene = Scene::cornell_box_refractive_sphere_point_light(width, height, rays_per_pixel);

    // Geometric figures
    //Scene scene = Scene::cornell_box_cono(width, height, rays_per_pixel);
    //Scene scene = Scene::cornell_box_cilindro(width, height, rays_per_pixel);
    //Scene scene = Scene::cornell_box_triangulo(width, height, rays_per_pixel);

    // Transformed figures
    //Scene scene = Scene::cornell_box_elipsoide(width, height, rays_per_pixel);

    // Constructive solid geometry
    //Scene scene = Scene::cornell_box_circulo(width, height, rays_per_pixel);
    //Scene scene = Scene::cornell_box_constructive_solid_geometry(width, height, rays_per_pixel);

    // OBJs
    //Scene scene = Scene::stanford_dragon_untextured(width, height, rays_per_pixel);
    //Scene scene = Scene::stanford_dragon_pl(width, height, rays_per_pixel); // With a point light, you can feed it to the photonmapper
    //Scene scene = Scene::stanford_dragon_textured(width, height, rays_per_pixel);

    // Use of the complex camera models
    //Scene scene = Scene::camera_demo(width, height, rays_per_pixel);
    //Scene scene = Scene::camera_demo_fov(width, height, rays_per_pixel);

    // Textures
    //Scene scene = Scene::cornell_box_texturas(width, height, rays_per_pixel);

    // Contest scene (Lots of OBJs + textures + Constructive solid geometry + complex camera stuff)
    //Scene scene = Scene::cornell_box_twin_peaks(width, height, rays_per_pixel);

    /*******************************************
     * Available algorithms, uncomment all the *
     * lines below their respective comments   *
     *******************************************/

    // Pathtracing (without BVH)
    //render_multithreaded(scene);

    // Pathtracing (with BVH)
    BvhMethod method = CENTROID; // CENTROID, SORT (CENTROID produces better hierarchies)
    render_multithreaded_bvh(scene, method);

    // Photonmapping (without BVH)
    //PhotonmappingDirectLightMethod method = STORE_ALL_PHOTONS; // NEXT_EVENT_ESTIMATION, STORE_ALL_PHOTONS
    //PhotonmappingKernel kernel = CONE; // CONE, BOX, NORMALIZED_BOX, GAUSSIAN, NORMALIZED_GAUSSIAN
    //render_multithreaded_photonmapper(scene, kernel, method);

    // Photonmapping  (with BVH)
    //PhotonmappingDirectLightMethod method = NEXT_EVENT_ESTIMATION; // NEXT_EVENT_ESTIMATION, STORE_ALL_PHOTONS
    //PhotonmappingKernel kernel = NORMALIZED_GAUSSIAN; // CONE, BOX, NORMALIZED_BOX, GAUSSIAN, NORMALIZED_GAUSSIAN
    //BvhMethod bvh_method = CENTROID; // CENTROID, SORT (CENTROID produces better hierarchies)
    //render_multithreaded_photonmapper_bvh(scene, kernel, method, bvh_method);
}
