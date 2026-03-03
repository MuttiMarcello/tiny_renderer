#pragma once
#include <vector>
#include <string>
#include <cstdint>

// vec3 class declaration
class vec3{
    public:

    float x, y, z;

    vec3() : x(0), y(0), z(0) {};   // Default constructor
    vec3(float x, float y, float z);

    vec3 operator+(const vec3& v) const;
    vec3 operator-(const vec3& v) const;
    vec3 operator*(const float& s) const;

    vec3& operator+=(const vec3& v);
    vec3& operator-=(const vec3& v);
    vec3& operator*=(const float& s);

    float dot(const vec3& v) const;
    vec3 cross(const vec3& v) const;
    float norm() const;

    vec3& normalize();  // never used, but implemented for completeness
    vec3 normalized() const;
};

// DCM class declaration
class DCM{
    public:

    vec3 v1, v2, v3;

    DCM() : v1(1, 0, 0), v2(0, 1, 0), v3(0, 0, 1) {};   // Default constructor initializes to identity DCM
    DCM(vec3 v1, vec3 v2, vec3 v3);

    /**
     * Add rotation matrix construction
     */

};

// ray class declaration
class ray{
    public:

    vec3 origin;
    vec3 direction;

    ray(const vec3& origin, const vec3& direction);

    vec3 at(float t) const;
};

// image class declaration
class image{
    public:

    int width, height;
    std::vector<std::uint8_t> rgb;

    image(int width, int height);

    void set_pixel(int x, int y, std::uint8_t r, std::uint8_t g, std::uint8_t b);

    bool write_ppm(const std::string& filepath) const;
};

// gradient image generation function declaration
void gradient_image(int width, int height, const std::string& filepath);

// intersection record struct declaration
class intersection_record {
    public:

    float t;
    vec3 point;
    vec3 normal;
};

// class object declaration
class object {
    public:

    virtual bool intersect(const ray& ray, float t_min, float t_max, intersection_record& rec) const = 0;
};

// sphere object declaration
class sphere : object{
    public:

    vec3 center;
    float radius;

    sphere(const vec3& center, float radius);

    bool intersect(const ray& ray, float t_min, float t_max, intersection_record& rec) const;
};

// camera class declaration
class camera {
    virtual ray get_ray(float u, float v) const = 0;
};

// pinhole camera class declaration
class pinhole_cam : public camera {
    public:

    vec3 position;
    DCM orientation;
    float fov;
    float aspect_ratio;
    float focal_length;

    pinhole_cam(
        const vec3& position,
        const DCM& orientation,
        float fov,
        float aspect_ratio,
        float focal_length);

    ray get_ray(float u, float v) const override;

    private:
    vec3 origin;
    vec3 lower_left_corner;
    vec3 horizontal;
    vec3 vertical;
    // vec3 forward_direction;
    // vec3 up_direction;
    // vec3 right_direction;
};

// light class declaration
class light {};

// point light class declaration
class point_light : public light {
    public:

    vec3 position;
    float intensity;

    point_light(const vec3& position, float intensity);
};

// gradient shader function declaration
inline void gradient_shader(const sphere& sphere, image& img, const ray& ray, intersection_record& rec, int x, int y);

// lambertian shader function declaration
inline void lambertian_shader(const sphere& sphere, image& img, const point_light& light, const ray& ray, intersection_record& rec, int x, int y);

// thread worker function declaration
static inline void worker_rows(const pinhole_cam& cam, const sphere& sph, image& img, const point_light& light, int y0, int y1);

// rendering function declaration
void render(const pinhole_cam& cam, const sphere& sph, image& img, const point_light& light);