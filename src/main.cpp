#include <iostream>
#include <chrono>
#include "tools.hpp"

int main() {

    // Camera parameters
    int image_width = 1920;
    int image_height = 1080;
    float fov = 60.0f;
    float focal_length = 1.0f;

    vec3 cam_position(0, 0, 0);
    DCM cam_orientation(vec3(1, 0, 0), vec3(0, 1, 0), vec3(0, 0, 1)); // Identity orientation (looking along +X)

    // Sphere parameters
    vec3 sphere_center(5, 1, 1);
    float sphere_radius = 1.0f;

    float aspect_ratio = static_cast<float>(image_width) / static_cast<float>(image_height);

    // Create camera, sphere, and image objects
    pinhole_cam cam(cam_position, cam_orientation, fov, aspect_ratio, focal_length);
    sphere sph(sphere_center, sphere_radius);
    image img(image_width, image_height);

    // Start timing the rendering process
    auto start_time = std::chrono::high_resolution_clock::now();

    // Render the scene
    render(cam, sph, img);

    // Write the rendered image to file
    img.write_ppm("render.ppm"); 
    
    // End timing and calculate duration
    auto end_time = std::chrono::high_resolution_clock::now();
    long long duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count();
    
    std::cout << "Rendering completed in " << duration << " microseconds\n";
    std::cout << "FPS: " << (1e6f / duration) << "\n";

    return 0;
}