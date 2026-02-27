#include <iostream>
#include "tools.hpp"

int main() {

    // vec3 p(0, 0, 0);
    // vec3 d(1, 0, 0);

    // ray r(p, d);

    // std::cout << "Ray origin: (" << r.origin.x << ", " << r.origin.y << ", " << r.origin.z << ")\n";
    // std::cout << "Ray direction: (" << r.direction.x << ", " << r.direction.y << ", " << r.direction.z << ")\n";

    // vec3 pn= r.at(5.0f);
    // std::cout << "Point on ray at t=5: (" << pn.x << ", " << pn.y << ", " << pn.z << ")\n";

    gradient_image(256, 256, "grad.ppm");

    return 0;
}