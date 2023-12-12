//  
// BVH.hpp
// 
// Description:
//  Simple Bounding Volume Hierarchies implementation, based on:
//  https://raytracing.github.io/books/RayTracingTheNextWeek.html#boundingvolumehierarchies
// 
//
// Authors:
//  Samuel García
//  Laura González
//
// Date:
//  12/2022.
//

#ifndef INFORMATICA_GRAFICA_BVH_HPP
#define INFORMATICA_GRAFICA_BVH_HPP

#include <memory>
#include <random>
#include "Figure.hpp"
#include "../../lib/Scene.hpp"

enum BvhMethod {
    SORT,
    CENTROID
};

class BVH : public Figure {
public:
    explicit BVH(const Scene &scene, BvhMethod method) : BVH(scene.figures, 0, scene.figures.size(), method, true) {
        watcher.join();
    }

    BVH(const std::vector<std::shared_ptr<Figure>> &figures, size_t start, size_t end, BvhMethod method, bool first_call) {
        if (first_call) watcher = std::thread(watch_bvh_construction, end+1);

        auto objects = figures;

        //size_t axis = random_axis();

        // Small improvement vs. using a random axis:
        //  We want to find a partition without too many overlapping bounding boxes,
        //  so we can choose the axis with the biggest centroid range, assuming that
        //  its figures are more spread out across that axis
        Bounds3d centroid_bounds;
        for (unsigned long i = start; i < end; ++i) {
            Bounds3d bounds = figures[i]->bounds();
            Vector3d centroid = bounds.p_min.v/2 + bounds.p_max.v/2;
            centroid_bounds = centroid_bounds.Union(Point(centroid));
        }

        size_t axis = centroid_bounds.maximum_extent(); // 0 (x), 1(y) or 2(z)


        auto comparator = (axis == 0) ? box_x_compare
                        : (axis == 1) ? box_y_compare
                        : box_z_compare;

        size_t object_span = end - start;

        if (object_span == 1) {
            figures_inserted++;

            // Simplifies construction at the cost of colliding twice here
            left = right = objects[start];
        } else if (object_span == 2) {
            figures_inserted += 2;

            // Smaller bounding box on the left size, bigger on the right
            if (comparator(objects[start], objects[start+1])) {
                left = objects[start];
                right = objects[start+1];
            } else {
                left = objects[start+1];
                right = objects[start];
            }
        } else {
            size_t mid = start + object_span/2;

            if (method == SORT) {
                // Sort all figures from lesser to greater position on the current axis
                std::sort(objects.begin() + start, objects.begin() + end, comparator);
            } else if (method == CENTROID) {
                // Sort all figures based on the midpoint rule of the centroids
                double pmid = (centroid_bounds.p_min[axis] + centroid_bounds.p_max[axis] ) / 2;
                auto mid_ptr =
                        std::partition(&objects[start], &objects[end - 1] + 1,
                                       [axis, pmid](const std::shared_ptr<Figure> &f) {
                                           auto bounds_f = f->bounds();
                                           auto centroid = bounds_f.p_min.v/2 + bounds_f.p_max.v/2;
                                           return centroid[axis] < pmid;
                                       });

                mid = mid_ptr - &objects[0];

                // If sorting failed, use the original method
                if (mid == end || mid == start) {
                    mid = start + object_span/2;
                    std::sort(objects.begin() + start, objects.begin() + end, comparator);
                }
            }

            left = std::make_shared<BVH>(objects, start, mid, method, false);
            right = std::make_shared<BVH>(objects, mid, end, method, false);
        }

        Bounds3d box_left = left->bounds();
        Bounds3d box_right = right->bounds();
        box = box_left.Union(box_right);
    }

    HitRegister collides(const Ray &ray) const override {
        if (!std::get<0>(box.collides(ray))) return {};

        HitRegister hit_left = left->collides(ray);
        HitRegister hit_right = right->collides(ray);

        if (hit_left.hits && hit_right.hits) {
            if (hit_left.t < hit_right.t) return hit_left;
            else return hit_right;
        }
        else if (hit_left.hits) return hit_left;
        else if (hit_right.hits) return hit_right;
        else return {};
    }

    Bounds3d bounds() const override {
        return box;
    }

private:
    inline size_t random_axis() {
        std::mt19937 gen = std::mt19937((std::random_device()()));
        std::uniform_int_distribution<size_t> axis_distr = std::uniform_int_distribution<size_t>(0, 2);

        return axis_distr(gen);
    }

    static inline bool box_compare(const std::shared_ptr<Figure> &a, const std::shared_ptr<Figure> &b, int axis) {
        return a->bounds().p_min[axis] < b->bounds().p_min[axis];
    }


    static bool box_x_compare (const std::shared_ptr<Figure> &a, const std::shared_ptr<Figure> &b) {
        return box_compare(a, b, 0);
    }

    static bool box_y_compare (const std::shared_ptr<Figure> &a, const std::shared_ptr<Figure> &b) {
        return box_compare(a, b, 1);
    }

    static bool box_z_compare (const std::shared_ptr<Figure> &a, const std::shared_ptr<Figure> &b) {
        return box_compare(a, b, 2);
    }

    static void watch_bvh_construction(size_t figures) {
        while (figures_inserted < figures-1) {
            std::cout << '\r' << "Stored " << figures_inserted << "/" << figures << " figures in the BVH tree...";
            std::flush(std::cout);

            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
        std::cout << '\r' << "Stored " << figures << "/" << figures << " figures in the BVH tree...";
        std::flush(std::cout);
        std::cout << "Finished!" << std::endl;
    }

public:
    std::shared_ptr<Figure> left;
    std::shared_ptr<Figure> right;
    Bounds3d box;

    static std::atomic<size_t> figures_inserted;
    static std::thread watcher;
};

std::thread BVH::watcher = std::thread();
std::atomic<size_t> BVH::figures_inserted = std::atomic<size_t>(0);

#endif //INFORMATICA_GRAFICA_BVH_HPP
