#include "tools.hpp"
#include <cmath>
#include <fstream>
#include <iostream>
#include <cstdint>
#include <thread>

// vec3 class member function definitions
vec3::vec3(float x, float y, float z) : x(x), y(y), z(z) {}

// Operator overloads
// rvalue operators
vec3 vec3::operator+(const vec3& v) const {
    return vec3(x + v.x, y + v.y, z + v.z);
}

vec3 vec3::operator-(const vec3& v) const {
    return vec3(x - v.x, y - v.y, z - v.z);
}

vec3 vec3::operator*(const float& s) const {
    return vec3(x * s, y * s, z * s);
}

// lvalue operators
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

vec3& vec3::normalize() {   // never used, but implemented for completeness
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

// DCM class member function definitions
DCM::DCM(vec3 v1, vec3 v2, vec3 v3) : v1(v1.normalized()), v2(v2.normalized()), v3(v3.normalized()) {};

// ray class member function definitions
ray::ray(const vec3& origin, const vec3& direction) : origin(origin), direction(direction.normalized()) {}

vec3 ray::at(float t) const {
    return origin + direction * t;
}

// image class member function definitions
image::image(int width, int height) : width(width), height(height), rgb(static_cast<size_t>(width * height * 3), 0) {}   // Initialize rgb vector with zeros

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

    const float width_inv = 1.0f / static_cast<float>(img.width - 1);
    const float height_inv = 1.0f / static_cast<float>(img.height - 1);

    for (int y=0; y<img.height; ++y) {
        float yf = static_cast<float>(y) * height_inv;
        for (int x=0; x<img.width; ++x) {
            float xf = static_cast<float>(x) * width_inv;
            std::uint8_t r = static_cast<std::uint8_t>(255.0f * xf);
            std::uint8_t g = static_cast<std::uint8_t>(255.0f * yf);
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
    rec.normal = ((rec.point - center) * (1.0f / radius)).normalized();

    return true;
}

// pinhole camera class member function definitions
pinhole_cam::pinhole_cam(
    const vec3& position,
    const DCM& orientation,
    float fov,
    float aspect_ratio,
    float focal_length) :
    position(position),
    orientation(orientation),
    fov(fov),
    aspect_ratio(aspect_ratio),
    focal_length(focal_length) {

        // origin is the camera position
        origin = position;

        // camera directions are the columns of the DCM
        vec3 forward_direction = orientation.v1;
        vec3 right_direction = orientation.v2;
        vec3 up_direction = orientation.v3;

        float theta = fov * M_PI / 180.0f;
        float half_height = std::tan(theta / 2.0f) * focal_length;
        float half_width = aspect_ratio * half_height;

        vec3 view_point_center = origin + forward_direction * focal_length;

        horizontal = right_direction * (2.0f * half_width);
        vertical = up_direction * (2.0f * half_height);
        lower_left_corner = view_point_center - horizontal * 0.5f - vertical * 0.5f;
    };

ray pinhole_cam::get_ray(float u, float v) const {
    vec3 pixel_position = lower_left_corner + horizontal * u + vertical * v;
    vec3 direction = (pixel_position - origin);
    return ray(origin, direction);
};

// thread worker function definition
static inline void worker_rows(const pinhole_cam* cam, const sphere* sph, image* img, int y0, int y1) {
    
    const float inv_width = 1.0f / static_cast<float>(img->width - 1);
    const float inv_height = 1.0f / static_cast<float>(img->height - 1);

    for (int y = y0; y < y1; ++y){
        for (int x = 0; x < img->width; ++x) {
            float u = static_cast<float>(x) * inv_width;
            float v = static_cast<float>(y) * inv_height;

            // Address sign convention for correct image orientation
            ray ray = cam->get_ray(1.0f - u, 1.0f - v);

            intersection_record rec;
            if (sph->intersect(ray, 1e-3f, 1e30f, rec)) {
                // Simple shading based on normal
                vec3 n = rec.normal;
                std::uint8_t r = static_cast<std::uint8_t>(255.0f * (n.x + 1.0f) * 0.5f);
                std::uint8_t g = static_cast<std::uint8_t>(255.0f * (n.y + 1.0f) * 0.5f);
                std::uint8_t b = static_cast<std::uint8_t>(255.0f * (n.z + 1.0f) * 0.5f);
                img->set_pixel(x, y, r, g, b);
            } else {
                // Background color
                img->set_pixel(x, y, 135, 206, 235); // Sky blue
            }
        }
    }
};

// rendering function declaration
void render(const pinhole_cam& cam, const sphere& sph, image& img) {

    if (img.width <= 1 || img.height <= 1) return;

    unsigned int num_threads = std::thread::hardware_concurrency();
    if (num_threads == 0) num_threads = 4; // Fallback to 4 threads if hardware_concurrency cannot determine
    num_threads = std::min(num_threads, static_cast<unsigned int>(img.height)); // Limit threads to image height

    std::cout << "Using " << num_threads << " threads for rendering\n";

    /** Original inline worker function implementation, moved to worker_rows
     *  Kept for performance assessment
     */
    //
    // auto worker = [&](int y0, int y1) {
    //     const float inv_width = 1.0f / static_cast<float>(img.width - 1);
    //     const float inv_height = 1.0f / static_cast<float>(img.height - 1);
    //     for (int y = y0; y < y1; ++y){
    //         for (int x = 0; x < img.width; ++x) {
    //             float u = static_cast<float>(x) * inv_width;
    //             float v = static_cast<float>(y) * inv_height;
    //             // Address sign convention for correct image orientation
    //             ray ray = cam.get_ray(1.0f - u, 1.0f - v);
    //             intersection_record rec;
    //             if (sph.intersect(ray, 1e-3f, 1e30f, rec)) {
    //                 // Simple shading based on normal
    //                 vec3 n = rec.normal;
    //                 std::uint8_t r = static_cast<std::uint8_t>(255.0f * (n.x + 1.0f) * 0.5f);
    //                 std::uint8_t g = static_cast<std::uint8_t>(255.0f * (n.y + 1.0f) * 0.5f);
    //                 std::uint8_t b = static_cast<std::uint8_t>(255.0f * (n.z + 1.0f) * 0.5f);
    //                 img.set_pixel(x, y, r, g, b);
    //             } else {
    //                 // Background color
    //                 img.set_pixel(x, y, 135, 206, 235); // Sky blue
    //             }
    //         }
    //     }
    // };

    std::vector<std::thread> threads;
    threads.reserve(num_threads);

    const int rows_per_thread = img.height / num_threads;
    int y_start = 0;

    for (int t=0; t<num_threads; ++t) {
        int y_end = (t == num_threads - 1) ? img.height : (y_start + rows_per_thread);        
        threads.emplace_back(worker_rows, &cam, &sph, &img, y_start, y_end);
        y_start = y_end;
    }

    for (auto& th : threads) th.join();
};