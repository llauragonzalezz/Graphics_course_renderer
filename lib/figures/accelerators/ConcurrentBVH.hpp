//  
// ConcurrentBVH.hpp
// 
// Description:
//  Bounding Volume Hierarchies implementation with multithreading during construction
//  Note: This has not been tested AT ALL
//
// Authors:
//  Samuel García
//  Laura González
//
// Date:
//  12/2022.
//

#ifndef INFORMATICA_GRAFICA_CONCURRENT_BVH_HPP
#define INFORMATICA_GRAFICA_CONCURRENT_BVH_HPP

#include <memory>
#include <random>
#include <thread>
#include "Figure.hpp"
#include "../../lib/Scene.hpp"
#include "../../aux/Channel.hpp"

enum ConcurrentBVHMethod {
    SORT,
    CENTROID
};

class ConcurrentBVH : public Figure {
private:
    struct ConcurrentBVHMessage {
        std::vector<std::shared_ptr<Figure>> figures;
        size_t start;
        size_t end;
        ConcurrentBVHMethod method;
        bool left;
        std::shared_ptr<Channel<std::pair<std::shared_ptr<ConcurrentBVH>, bool>>> result;
    };

public:
    explicit ConcurrentBVH(const Scene &scene, ConcurrentBVHMethod method, size_t number_of_threads = 0) {
        watcher = std::thread(&ConcurrentBVH::watch_ConcurrentBVH_construction, this, scene.figures.size());

        build_ConcurrentBVH(scene.figures, 0, scene.figures.size(), method, number_of_threads);

        watcher.join();
    }

public:
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

    ConcurrentBVH(const std::vector<std::shared_ptr<Figure>> &figures, size_t start, size_t end, ConcurrentBVHMethod method, size_t number_of_threads) {
        build_ConcurrentBVH(figures, start, end, method, number_of_threads);
    }

private:


    void build_ConcurrentBVH(const std::vector<std::shared_ptr<Figure>> &figures, size_t start, size_t end, ConcurrentBVHMethod method, size_t number_of_threads) {
        std::vector<std::shared_ptr<Figure>> objects = std::vector<std::shared_ptr<Figure>>(&figures[start], &figures[end]); // Copia del vector que se va a modificar (ordenándolo)

        //size_t axis = random_axis();

        // Small improvement vs. using a random axis:
        //  We want to find a partition without too many overlapping bounding boxes,
        //  so we can choose the axis with the biggest centroid range, assuming that
        //  its figures are more spread out across that axis
        Bounds3d centroid_bounds;
        for (const auto &f : objects) {
            Bounds3d bounds = f->bounds();
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
            left = right = objects[0];
        } else if (object_span == 2) {
            figures_inserted += 2;

            // Smaller bounding box on the left size, bigger on the right
            if (comparator(objects[0], objects[1])) {
                left = objects[0];
                right = objects[1];
            } else {
                left = objects[1];
                right = objects[0];
            }
        } else {
            size_t mid = object_span/2;

            if (method == SORT) {
                // Sort all figures from lesser to greater position on the current axis
                std::sort(objects.begin(), objects.end(), comparator);
            } else if (method == CENTROID) {
                // Sort all figures based on the midpoint rule of the centroids
                double pmid = (centroid_bounds.p_min[axis] + centroid_bounds.p_max[axis] ) / 2;
                auto mid_ptr =
                        std::partition(&objects[0], &objects[objects.size() - 1] + 1,
                                       [axis, pmid](const std::shared_ptr<Figure> &f) {
                                           auto bounds_f = f->bounds();
                                           auto centroid = bounds_f.p_min.v/2 + bounds_f.p_max.v/2;
                                           return centroid[axis] < pmid;
                                       });

                mid = mid_ptr - &objects[0];

                // If sorting failed, use the original method
                if (mid == objects.size() - 1 || mid == 0) {
                    mid = object_span/2;
                    std::sort(objects.begin(), objects.end(), comparator);
                }
            }

            if (number_of_threads < (std::thread::hardware_concurrency() - 1)) {
                auto chan = std::make_shared<Channel<std::pair<std::shared_ptr<ConcurrentBVH>, bool>>>();
                ConcurrentBVHMessage msg_left = ConcurrentBVHMessage{std::ref(objects), 0, mid, method, true, chan};
                auto th_left = std::thread(&ConcurrentBVH::ConcurrentBVH_worker_job, this, std::move(msg_left), number_of_threads + 2);

                ConcurrentBVHMessage msg_right = ConcurrentBVHMessage{std::ref(objects), mid, objects.size(), method,
                                                                      false, chan};
                auto th_right = std::thread(&ConcurrentBVH::ConcurrentBVH_worker_job, this, std::move(msg_right), number_of_threads + 2);

                objects.clear();

                auto msg = chan->receive();
                if (msg.second) { // Left
                    left = msg.first;
                    th_left.join();
                } else {
                    right = msg.first;
                    th_right.join();
                }

                msg = chan->receive();
                if (msg.second) { // Left
                    left = msg.first;
                    th_left.join();
                } else {
                    right = msg.first;
                    th_right.join();
                }
            } else {
                left = std::make_shared<ConcurrentBVH>(objects, start, mid, method, false);
                right = std::make_shared<ConcurrentBVH>(objects, mid, end, method, false);
            }
        }

        Bounds3d box_left = left->bounds();
        Bounds3d box_right = right->bounds();
        box = box_left.Union(box_right);
    }

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

    void watch_ConcurrentBVH_construction(size_t figures) {
        while (figures_inserted < figures) {
            std::cout << '\r' << "Stored " << figures_inserted << "/" << figures << " figures in the ConcurrentBVH tree...";
            std::flush(std::cout);

            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
        std::cout << '\r' << "Stored " << figures << "/" << figures << " figures in the ConcurrentBVH tree...";
        std::flush(std::cout);
        std::cout << "Finished!" << std::endl;
    }

    void ConcurrentBVH_worker_job(ConcurrentBVHMessage job, size_t number_of_threads) {
        auto concurrentBVH = std::make_shared<ConcurrentBVH>(job.figures, job.start, job.end, job.method, number_of_threads);
        job.result->send(std::make_pair(std::move(concurrentBVH), job.left));
    }

public:
    std::shared_ptr<Figure> left;
    std::shared_ptr<Figure> right;
    Bounds3d box;

    static std::atomic<size_t> figures_inserted;
    std::thread watcher;
    Channel<ConcurrentBVHMessage> job_channel;
};

std::atomic<size_t> ConcurrentBVH::figures_inserted = 0;

#endif //INFORMATICA_GRAFICA_CONCURRENT_BVH_HPP
