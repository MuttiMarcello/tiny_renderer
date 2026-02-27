#include <iostream>
#include "tools.hpp"

int main() {

    vec3 v1(1, 0, 0);
    vec3 v2(0, 1, 0);

    vec3 v3 = v1 + v2; // This will require an operator+ to be defined
    std::cout << "v3: (" << v3.x << ", " << v3.y << ", " << v3.z << ")\n";

    std::cout << "Dot product: " << v1.dot(v2) << "\n";
    std::cout << "Norm of v3: " << v3.norm() << "\n";

    v1*= 2; // This will require an operator*= to be defined
    std::cout << "v1 after scaling: (" << v1.x << ", " << v1.y << ", " << v1.z << ")\n";
    v1.normalize(); // This will require a normalize() function to be defined
    std::cout << "v1 after normalization: (" << v1.x << ", " << v1.y << ", " << v1.z << ")\n";

    return 0;
}