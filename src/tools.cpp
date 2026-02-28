#include "tools.hpp"
#include <cmath>
#include <fstream>
#include <iostream>
#include <cstdint>

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
float vec3::dot(const vec3& v) const {
    return x * v.x + y * v.y + z * v.z;
}

vec3 vec3::cross(const vec3& v) const {
    return vec3(
        y * v.z - z * v.y,
        z * v.x - x * v.z,
        x * v.y - y * v.x
    );
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

vec3 ray::at(float t) const {
    return origin + direction * t;
}

// image class member function definitions
image::image(int width, int height) : width(width), height(height), rgb(width * height * 3, 0) {}

void image::set_pixel(int x, int y, std::uint8_t r, std::uint8_t g, std::uint8_t b) {
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

    const float width_inv = 1.0f / float(img.width - 1);
    const float height_inv = 1.0f / float(img.height - 1);

    for (int y=0; y<img.height; ++y) {
        float yf = (float)y * height_inv;
        for (int x=0; x<img.width; ++x) {
            float xf = (float)x * width_inv;
            std::uint8_t r = (std::uint8_t)(255.0f * xf);
            std::uint8_t g = (std::uint8_t)(255.0f * yf);
            std::uint8_t b = 0;
            img.set_pixel(x, y, r, g, b);
        }
    }

    if (img.write_ppm(filepath)) {
        std::cout << "Wrote " << filepath << "\n";
    } else {
        std::cerr << "Failed to write " << filepath << "\n";
    }

}

// sphere object definition
sphere::sphere(const vec3& center, float radius) : center(center), radius(radius) {};

bool sphere::intersect(const ray& ray, float t_min, float t_max, intersection_record& rec) const {

    vec3 origin_center = ray.origin - center;   // origin with respect to sphere center

    float half_b = origin_center.dot(ray.direction);
    float c = origin_center.dot(origin_center) - radius * radius;

    float disc = half_b * half_b - c;
    if (disc < 0) return false; // No intersection

    float s = std::sqrt(disc);

    // Try nearest root first
    float t = -half_b-s;
    if (t < t_min || t > t_max) {
        t = -half_b + s;
        if (t < t_min || t > t_max) {
            return false; // No valid intersection
        }
    }

    rec.t = t;
    rec.point = ray.at(t);
    rec.normal = (rec.point - center) * (1.0f / radius);

    return true;
}

// camera class member function definitions
camera::camera(
    const vec3& position,
    const vec3& forward_direction,
    float fov,
    float aspect_ratio,
    float focal_length) :
    position(position),
    forward_direction(forward_direction.normalized()),
    fov(fov),
    aspect_ratio(aspect_ratio),
    focal_length(focal_length) {

        // origin is the camera position
        origin = position;

        vec3 r = forward_direction.cross(scene_up);
        if (r.norm() == 0) {
            r = vec3(1, 0, 0); // If forward is parallel to up, choose a different right direction
        } else {
            r = r.normalized();
        }
        right_direction = r;
        up_direction = right_direction.cross(forward_direction).normalized();

        float theta = fov * M_PI / 180.0f;
        float half_height = std::tan(theta / 2.0f) * focal_length;
        float half_width = aspect_ratio * half_height;

        vec3 view_point_center = origin + forward_direction * focal_length;

        horizontal = right_direction * (2.0f * half_width);
        vertical = up_direction * (2.0f * half_height);
        lower_left_corner = view_point_center - horizontal * 0.5f - vertical * 0.5f;
    };

ray camera::get_ray(float u, float v) const {
    vec3 pixel_position = lower_left_corner + horizontal * u + vertical * v;
    vec3 direction = (pixel_position - origin);
    return ray(origin, direction);
};

// rendering function declaration
void render(const camera& cam, const sphere& sph, image& img) {

    if (img.width <= 0 || img.height <= 0) return;

    for (int y=0; y<img.height; ++y) {
        for (int x=0; x<img.width; ++x) {    

            float u = (float)x / float(img.width - 1);
            float v = (float)y / float(img.height - 1);

            // Explore why flipped v coordinate is needed for correct image orientation
            ray ray = cam.get_ray(u, 1.0f - v);

            intersection_record rec;
            if (sph.intersect(ray,1e-3f, 1e30f, rec)) {
                // Simple shading based on normal
                vec3 n = rec.normal;
                std::uint8_t r = (std::uint8_t)(255.0f * (n.x + 1.0f) * 0.5f);
                std::uint8_t g = (std::uint8_t)(255.0f * (n.y + 1.0f) * 0.5f);
                std::uint8_t b = (std::uint8_t)(255.0f * (n.z + 1.0f) * 0.5f);
                img.set_pixel(x, y, r, g, b);
            } else {
                // Background color
                img.set_pixel(x, y, 135, 206, 235); // Sky blue
            }
        }
    }
    // Write the rendered image to file
    img.write_ppm("render.ppm"); 
};