cmake_minimum_required(VERSION 3.22)
project(graphics_course_renderer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-O3 -Wall -Wextra -march=native")

if (BENCHMARKING)
    add_definitions(-Dbenchmarking=1)
endif (BENCHMARKING)

include_directories(vector3d point direction transformation_matrix lib/aux planetas ppm tonemapper lib/figures lib/figures/accelerators ray lib/camera renderer lib/pathtracing)

add_executable(graphics_course_renderer
        main.cpp

        lib/math/Vector3d.hpp
        lib/math/Point.hpp
        lib/math/Direction.hpp
        lib/Ray.hpp
        lib/Photon.hpp
        lib/math/TransformationMatrix.hpp

        lib/pathtracing/pathtracing.hpp

        lib/photonmapping/photonmapping.hpp
        lib/photonmapping/photonmapping_kdtree.hpp

        lib/Scene.hpp

        lib/PointLight.hpp

        lib/camera/camera.hpp
        lib/camera/camera.cpp

        lib/Ppm.hpp
        lib/tonemapper.hpp

        lib/figures/Figure.hpp
        lib/figures/Sphere.hpp
        lib/figures/Plane.hpp
        lib/figures/Cylinder.hpp
        lib/figures/Disk.hpp
        lib/figures/Ellipsoid.hpp
        lib/figures/Triangle.hpp
        lib/figures/Cone.hpp
        lib/figures/HitRegister.hpp
        lib/figures/ObjMod.hpp
        lib/figures/TransformedFigure.hpp
        lib/figures/ConstructiveSolidDifference.hpp
        lib/figures/ConstructiveSolidUnion.hpp
        lib/figures/ConstructiveSolidIntersection.hpp
        lib/figures/Texture.hpp
        lib/figures/Bounds3d.hpp
        lib/figures/accelerators/BVH.hpp
        lib/figures/accelerators/ConcurrentBVH.hpp

        renderer/renderer.hpp
        renderer/pathtracer/multithreaded_pathtracer.hpp
        renderer/pathtracer/multithreaded_pathtracer_bvh.hpp
        renderer/photonmapper/multithreaded_photonmapper.hpp
        renderer/photonmapper/multithreaded_photonmapper_bvh.hpp

        lib/aux/aux.hpp
        lib/aux/Channel.hpp
        lib/aux/benchmarking.hpp
        lib/aux/benchmarking.cpp
        lib/aux/ThreadSafeCounter.hpp)
