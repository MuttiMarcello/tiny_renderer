#include <iostream>
#include "tools.hpp"

int main() {

    int image_width = 768;
    int image_height = 512;
    float fov = 60.0f;
    float focal_length = 1.0f;

    float aspect_ratio = (float)image_width / (float)image_height;

    camera cam(vec3(0, 0, 0), vec3(1, 0, 0), fov, aspect_ratio, focal_length);
    sphere sph(vec3(5, 1, 1), 1.0f);
    image img(image_width, image_height);

    std::cout << "Camera position: (" << cam.position.x << ", " << cam.position.y << ", " << cam.position.z << ")\n";
    std::cout << "Camera forward direction: (" << cam.forward_direction.x << ", " << cam.forward_direction.y << ", " << cam.forward_direction.z << ")\n";
    
    std::cout << "Sphere center: (" << sph.center.x << ", " << sph.center.y << ", " << sph.center.z << ")\n";

    render(cam, sph, img);

    return 0;
}