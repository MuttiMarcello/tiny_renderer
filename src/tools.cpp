#include "tools.hpp"
#include <cmath>
#include <fstream>
#include <iostream>

// vec3 class member function definitions
vec3::vec3(float x, float y, float z) : x(x), y(y), z(z) {}

// Operator overloads
vec3 vec3::operator+(const vec3& v) const {
    return vec3(x + v.x, y + v.y, z + v.z);
}

vec3 vec3::operator-(const vec3& v) const {
    return vec3(x - v.x, y - v.y, z - v.z);
}

vec3 vec3::operator*(const float& s) const {
    return vec3(x * s, y * s, z * s);
}

vec3& vec3::operator+=(const vec3& v) {
    x += v.x;
    y += v.y;
    z += v.z;
    return *this;
}

vec3& vec3::operator-=(const vec3& v) {
    x -= v.x;
    y -= v.y;
    z -= v.z;
    return *this;
}

vec3& vec3::operator*=(const float& s) {
    x *= s;
    y *= s;
    z *= s;
    return *this;
}

// Vector operations
float vec3::dot(const vec3& v) {
    return x * v.x + y * v.y + z * v.z;
}

float vec3::norm() const {
    return std::sqrt(x * x + y * y + z * z);
}

vec3& vec3::normalize() {
    float n = norm();
    if (n == 0) return *this; // Avoid division by zero
    x /= n;
    y /= n;
    z /= n;
    return *this;
}

vec3 vec3::normalized() const {
    float n = norm();
    if (n == 0) return *this; // Avoid division by zero
    return vec3(x / n, y / n, z / n);
}

// ray class member function definitions
ray::ray(const vec3& origin, const vec3& direction) : origin(origin), direction(direction.normalized()) {}

vec3 ray::at(float t) {
    return origin + direction * t;
}

// image class member function definitions
image::image(int width, int height) : width(width), height(height), rgb(width * height * 3, 0) {}

void image::set_pixel(int x, int y, u_int8_t r, u_int8_t g, u_int8_t b) {
    if (x<0 || x>=width || y<0 || y>=height) return; // Out of bounds check
    int id = 3 * (y * width + x);
    rgb[id + 0] = r;
    rgb[id + 1] = g;
    rgb[id + 2] = b;
}

bool image::write_ppm(const std::string& filepath) const {
    std::ofstream out(filepath, std::ios::binary);
    if (!out) return false;

    out << "P6\n" << width << " " << height << "\n255\n";
    out.write(reinterpret_cast<const char*>(rgb.data()), (std::streamsize)rgb.size());
    return true;
}

// gradient image generation function definition
void gradient_image(int width, int height, const std::string& filepath) {
    image img(width, height);

    for (int y=0; y<img.height; ++y) {
        for (int x=0; x<img.width; ++x) {
            float xf = (float)x / float(img.width - 1);
            float yf = (float)y / float(img.height - 1);
            u_int8_t r = (u_int8_t)(255.0f * xf);
            u_int8_t g = (u_int8_t)(255.0f * yf);
            u_int8_t b = 0;
            img.set_pixel(x, y, r, g, b);
        }
    }

    if (img.write_ppm(filepath)) {
        std::cout << "Wrote " << filepath << "\n";
    } else {
        std::cerr << "Failed to write " << filepath << "\n";
    }

}