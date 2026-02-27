#pragma once
#include <vector>
#include <string>

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
    float norm() const;

    vec3& normalize();
    vec3 normalized() const;
};

// ray class declaration
class ray{
    public:

    vec3 origin;
    vec3 direction;

    ray(const vec3& origin, const vec3& direction);

    vec3 at(float) const;
};

// image class declaration
class image{
    public:

    int width, height;
    std::vector<u_int8_t> rgb;

    image(int width, int height);

    void set_pixel(int x, int y, u_int8_t r, u_int8_t g, u_int8_t b);

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

// sphere object declaration
class sphere {
    public:

    vec3 center;
    float radius;

    sphere(const vec3& center, float radius);

    bool intersect(const ray& ray, float t_min, float t_max, intersection_record& rec) const;

};