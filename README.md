# Graphics_course_renderer
Basic renderer implementing both Pathtracing and Photonmapping algorithms developed for the Informática Gráfica 2022/2023 course at Universidad de Zaragoza, developed alongside ![@Sondeluz](https://github.com/Sondeluz)
![twin_peaks](https://github.com/llauragonzalezz/Graphics_course_renderer/blob/main/cool_renders/twin_peaks.png?raw=true)

![35_point_lights_final_render](https://github.com/llauragonzalezz/Graphics_course_renderer/blob/main/cool_renders/pathtracer/35_point_lights_final_render.png?raw=true)

## Features
- Highly multithreaded
- Uses acceleration structures (BVH trees)
- Basic support for .obj files and textures
- Implements Constructive Solid Geometry
## Requirements
- C++17
- CMake
## Building and running
- Tweak the main.cpp file to select your scene, the algorithm to use and the image qualty (resolution and rays per pixel)
- Run 'cmake build . && make'
## Note
Some documentation (algorithm explanations, etc.) have been removed from the code. Also, some comments in Spanish may have slipped in.
