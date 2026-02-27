#include <iostream>
#include "tools.hpp"

int main() {

    // gradient_image(256, 256, "grad.ppm");

    sphere s(vec3(0, 0, 5), 1.0f);
    ray r(vec3(0, 0, 0), vec3(0, 0, 1));

    intersection_record rec;
    if (s.intersect(r, 0.001f, 1e30f, rec)) {
        std::cout << "Hit at t=" << rec.t << "\n";
        std::cout << "Point: (" << rec.point.x << ", " << rec.point.y << ", " << rec.point.z << ")\n";
        std::cout << "Normal: (" << rec.normal.x << ", " << rec.normal.y << ", " << rec.normal.z << ")\n";
    } else {
        std::cout << "No hit\n";
    }

    return 0;
}