#include "tools.hpp"
#include <cmath>
#include <fstream>
#include <iostream>
#include <cstdint>
#include <thread>
#include <random>
#include <algorithm>

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

vec3 vec3::operator-() const {
    return vec3(-x, -y, -z);
}

vec3 vec3::operator*(const float& s) const {
    return vec3(x * s, y * s, z * s);
}

vec3 vec3::operator/(const float& s) const {
    return vec3(x / s, y / s, z / s);
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

// vec3& vec3::normalize() {   // never used, but implemented for completeness
//     float n = norm();
//     if (n == 0) return *this; // Avoid division by zero
//     x /= n;
//     y /= n;
//     z /= n;
//     return *this;
// }

vec3 vec3::normalized() const {
    float n = norm();
    if (n == 0) return *this; // Avoid division by zero
    return vec3(x / n, y / n, z / n);
}

vec3 vec3::reflect(const vec3& n) const {
    return vec3(x, y, z) - n * 2.0f * vec3(x, y, z).dot(n) / n.dot(n);
}

// DCM class member function definitions
DCM::DCM(vec3 v1, vec3 v2, vec3 v3) : v1(v1.normalized()), v2(v2.normalized()), v3(v3.normalized()) {};

// color class member funcion definitions
col3::col3(float r, float g, float b) : r(r), g(g), b(b) {};

col3 col3::operator*(const float& s) const {
    return col3(r * s, g * s, b * s);
}

col3 col3::operator*(const col3& c) const {
    return col3(r * c.r, g * c.g, b * c.b);
}

col3 col3::operator/(const float& s) const {
    return col3(r / s, g / s, b / s);
}

col3& col3::operator+=(const col3& c) {
    r += c.r;
    g += c.g;
    b += c.b;
    return *this;
}

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

// hit record member class definitions
void hit_record::set_face_normal(const ray& cast_ray, const vec3& out_normal) {
    front_face = cast_ray.direction.dot(out_normal) < 0;
    normal = front_face ? out_normal : (out_normal * -1);
};

// lambertian class member function definition
lambertian::lambertian(const col3& albedo) : albedo(albedo) {};

bool lambertian::scatter(const ray& in_ray, const hit_record& rec, col3& attenuation, ray& scattered) const {
    vec3 scatter_direction = rec.normal + rand_vec();

    if (scatter_direction.norm() < 1e-8f) {
        scatter_direction = rec.normal;
    }

    float epsilon = 1e-3f; // Small offset to avoid self-intersection
    scattered = ray(rec.point + rec.normal * epsilon, scatter_direction);
    attenuation = albedo;
    return true;
};

// metal class member function definition
metal::metal(const col3& albedo) : albedo(albedo) {};

bool metal::scatter(const ray& in_ray, const hit_record& rec, col3& attenuation, ray& scattered) const {
    vec3 reflected = in_ray.direction.reflect(rec.normal);

    float epsilon = 1e-3f; // Small offset to avoid self-intersection
    scattered = ray(rec.point + rec.normal * epsilon, reflected);
    attenuation = albedo;

    return (reflected.dot(rec.normal) > 0.0f);
};

// sphere class member function definitions
sphere::sphere(const vec3& center, float radius, std::shared_ptr<material> mat) : center(center), radius(radius), mat(mat) {};

bool sphere::hit(const ray& cast_ray, float t_min, float t_max, hit_record& rec) const {

    vec3 origin_center = cast_ray.origin - center;   // origin with respect to sphere center

    float half_b = origin_center.dot(cast_ray.direction);
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
    rec.point = cast_ray.at(t);
    vec3 out_normal = (rec.point - center) / radius;
    rec.set_face_normal(cast_ray, out_normal);
    rec.mat = mat;

    return true;
}

// hittable list class member function definitions
void hittable_list::add(std::shared_ptr<hittable> object) {
        objects.push_back(object);
};

bool hittable_list::hit(const ray& cast_ray, float t_min, float t_max, hit_record& rec) const {
    hit_record temp_rec;
    bool hit_anything = false;
    float closest_so_far = t_max;

    for (const auto& object : objects) {
        if (object->hit(cast_ray, t_min, closest_so_far, temp_rec)) {
            hit_anything = true;
            closest_so_far = temp_rec.t;
            rec = temp_rec; // Update the main record with the closest hit
        }
    }

    return hit_anything;
};  

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

// point light class member function definitions
point_light::point_light(const vec3& position, float intensity) : position(position), intensity(intensity) {};

// directional light class member function definitions
directional_light::directional_light(const vec3& direction, const col3& color, const float& radiance) : direction(direction.normalized()), color(color), radiance(radiance) {};

// random function definition
float randf01() {
    static thread_local std::mt19937 gen(std::random_device{}());
    static thread_local std::uniform_real_distribution<float> dist(0.0f, 1.0f);
    return dist(gen);
};

// clamp function definition
float clamp01(float x) {
    return std::min(1.0f, std::max(0.0f, x));
}

// random vector function definition
vec3 rand_vec() {
    return vec3(2.0f * randf01() - 1.0f, 2.0f * randf01() - 1.0f, 2.0f * randf01() - 1.0f).normalized();
}

// Reinhard tone mapping function definition
col3 reinhard_mapping(const col3& c) {
    return col3(
        c.r * (1 / (1.0f + c.r)),
        c.g * (1 / (1.0f + c.g)),
        c.b * (1 / (1.0f + c.b))
    );
}

// gamma correction function definition
col3 gamma_correction(const col3& c) {
    float gamma = 2.2f;
    float inv = 1.0f / gamma;
    return col3(
        std::min(1.0f, std::pow(clamp01(c.r), inv)),
        std::min(1.0f, std::pow(clamp01(c.g), inv)),
        std::min(1.0f, std::pow(clamp01(c.b), inv))
    );
}

// in shadow function definition
bool in_shadow(const vec3& point, const vec3& out_normal, const vec3& light_dir, const hittable_list& scene) {
    const float epsilon = 1e-3f;
    ray shadow_ray(point + out_normal * epsilon, light_dir);
    hit_record rec;
    return scene.hit(shadow_ray, epsilon, 1e30f, rec);
};

// ray color function definition
col3 ray_color(const ray& r, const hittable_list& scene, const directional_light& dir_light, int depth) {
    if (depth <= 0) {
        return col3(0.0f, 0.0f, 0.0f);
    }

    hit_record rec;
    if (!scene.hit(r, 1e-3f, 1e30f, rec)) {
        return col3(0.01f, 0.01f, 0.01f);  // background color
    }

    col3 color(0.0f, 0.0f, 0.0f);

    vec3 to_light = -dir_light.direction;

    if(!in_shadow(rec.point, rec.normal, to_light, scene)){
        float ndotl = std::max(0.0f, rec.normal.dot(to_light));
        color += (rec.mat->get_albedo() * dir_light.color) * (dir_light.radiance * ndotl);
    }
    
    ray scattered(vec3(0, 0, 0), vec3(1, 0, 0));
    col3 attenuation;

    if (rec.mat && rec.mat->scatter(r, rec, attenuation, scattered)) {
        color += attenuation * ray_color(scattered, scene, dir_light, depth -1);
    }
    
    return color;
};

// gradient shader function definition
inline col3 gradient_shader(const hittable_list& scene, image& img, const ray& cast_ray, hit_record& rec) {
    if (scene.hit(cast_ray, 1e-3f, 1e30f, rec)) {
        // Simple shading based on normal
        vec3 n = rec.normal;
        float r = 255.0f * (n.x + 1.0f) * 0.5f;
        float g = 255.0f * (n.y + 1.0f) * 0.5f;
        float b = 255.0f * (n.z + 1.0f) * 0.5f;
        return col3(r, g, b);
    } else {
        // Background color
        return col3(135, 206, 235); // Sky blue
    }
};

// masking shader function definition
inline col3 masking_shader(const hittable_list& scene, image& img, const point_light& light, const ray& cast_ray, hit_record& rec) {
    if (scene.hit(cast_ray, 1e-3f, 1e30f, rec)) {
        // Simple shading based on lambertian reflectance
        vec3 n = rec.normal;
        vec3 light_direction = light.position - rec.point;

        float epsilon = 1e-3f; // Small offset to avoid self-intersection
        ray shadow_ray(rec.point + n * epsilon, light_direction);

        // Shadow ray intersection test
        if (scene.hit(shadow_ray, 1e-3f, light_direction.norm(), rec)) {
            // In shadow, set pixel to black
            return col3();
        }

        return col3(255, 255, 255);
    } else {
        // Background color
        return col3(0.01f, 0.01f, 0.01f);
    }
};

// lambertian shader function definition
inline col3 lambertian_shader(const hittable_list& scene, image& img, const point_light& light, const ray& cast_ray, hit_record& rec) {
    if (scene.hit(cast_ray, 1e-3f, 1e30f, rec)) {
        // Simple shading based on lambertian reflectance
        vec3 n = rec.normal;
        vec3 light_direction = light.position - rec.point;

        float epsilon = 1e-3f; // Small offset to avoid self-intersection
        ray shadow_ray(rec.point + n * epsilon, light_direction);

        // Shadow ray intersection test
        if (scene.hit(shadow_ray, 1e-3f, light_direction.norm(), rec)) {
            // In shadow, set pixel to black
            return col3();
        }

        // Attenuation based on inverse square law
        float distance_squared = light_direction.dot(light_direction);
        float intensity_at_point = light.intensity / distance_squared;

        // Lambertian reflectance
        float diffused_brightness = std::max(0.0f, n.dot(light_direction.normalized())) * intensity_at_point;

        float rgb_value = diffused_brightness;
        return col3(rgb_value, rgb_value, rgb_value);
    } else {
        // Background color
        return col3(0.01f, 0.01f, 0.01f);
    }
};

// thread worker function definition
static inline void worker_rows(const pinhole_cam& cam, const hittable_list& scene, image& img, const directional_light& dir_light, int y0, int y1, int aa_N) {

    const float inv_width = 1.0f / static_cast<float>(img.width - 1);
    const float inv_height = 1.0f / static_cast<float>(img.height - 1);

    // Reuse intersection record for efficiency, since it's thread-local
    hit_record rec;

    for (int y = y0; y < y1; ++y){
        for (int x = 0; x < img.width; ++x){

            col3 rgb_acc;

            for (int aa_it = 0; aa_it < aa_N; ++aa_it){

                float offset_px = (aa_N == 1) ? 0.5f : randf01();
                float offset_py = (aa_N == 1) ? 0.5f : randf01();

                float u = (static_cast<float>(x) + offset_px) * inv_width;
                float v = (static_cast<float>(y) + offset_py) * inv_height;

                ray cast_ray = cam.get_ray(1.0f - u, 1.0f - v);

                constexpr int max_depth = 10;
                rgb_acc += ray_color(cast_ray, scene, dir_light, max_depth);

            }
            col3 rgb = rgb_acc / static_cast<float>(aa_N);      // [0, inf)
            col3 rgb_mapped = reinhard_mapping(rgb);            // [0, 1)
            col3 rgb_corrected = gamma_correction(rgb_mapped);  // [0, 1]

            img.set_pixel(x, y, 
                        static_cast<std::uint8_t>(rgb_corrected.r * 255.0f),
                        static_cast<std::uint8_t>(rgb_corrected.g * 255.0f),
                        static_cast<std::uint8_t>(rgb_corrected.b * 255.0f));
        }
    }
};

// rendering function declaration
void render(const pinhole_cam& cam, const hittable_list& scene, image& img, const directional_light& dir_light, int aa_N) {

    if (aa_N < 1) {
        aa_N = 1;
        std::cout << "Anti-aliasing samples set to 1\n";
    }

    if (img.width <= 1 || img.height <= 1) return;

    unsigned int num_threads = std::thread::hardware_concurrency();
    if (num_threads == 0) num_threads = 4; // Fallback to 4 threads if hardware_concurrency cannot determine
    num_threads = std::min(num_threads, static_cast<unsigned int>(img.height)); // Limit threads to image height

    std::cout << "Using " << num_threads << " threads for rendering\n";

    std::vector<std::thread> threads;
    threads.reserve(num_threads);

    const int rows_per_thread = img.height / num_threads;
    int y_start = 0;

    for (int t=0; t<num_threads; ++t) {
        int y_end = (t == num_threads - 1) ? img.height : (y_start + rows_per_thread);        
        threads.emplace_back(worker_rows, std::cref(cam), std::cref(scene), std::ref(img), std::cref(dir_light), y_start, y_end, aa_N);
        y_start = y_end;
    }

    for (auto& th : threads) th.join();
};