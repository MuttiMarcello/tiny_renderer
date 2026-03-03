#include <iostream>
#include <chrono>
#include "tools.hpp"

int main() {

    int image_width = 1920;
    int image_height = 1080;
    float fov = 60.0f;
    float focal_length = 1.0f;

    float aspect_ratio = static_cast<float>(image_width) / static_cast<float>(image_height);

    pinhole_cam cam(vec3(0, 0, 0), vec3(1, 0, 0.001), fov, aspect_ratio, focal_length);
    sphere sph(vec3(5, 1, 1), 1.0f);
    image img(image_width, image_height);

    std::cout << "Camera position: (" << cam.position.x << ", " << cam.position.y << ", " << cam.position.z << ")\n";
    std::cout << "Camera forward direction: (" << cam.forward_direction.x << ", " << cam.forward_direction.y << ", " << cam.forward_direction.z << ")\n";
    
    std::cout << "Sphere center: (" << sph.center.x << ", " << sph.center.y << ", " << sph.center.z << ")\n";

    auto start_time = std::chrono::high_resolution_clock::now();

    // Render the scene
    render(cam, sph, img);

    // Write the rendered image to file
    img.write_ppm("render.ppm"); 
    
    auto end_time = std::chrono::high_resolution_clock::now();
    long long duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count();
    std::cout << "Rendering completed in " << duration << " microseconds\n";
    std::cout << "FPS: " << (1e6f / duration) << "\n";

    return 0;
}