#pragma once
#include <vector>
#include <string>
#include <cstdint>
#include <memory>

// vec3 class declaration
class vec3{
    public:

    float x, y, z;

    vec3() : x(0), y(0), z(0) {};   // Default constructor
    vec3(float x, float y, float z);

    vec3 operator+(const vec3& v) const;
    vec3 operator-(const vec3& v) const;
    vec3 operator-() const;
    vec3 operator*(const float& s) const;
    vec3 operator/(const float& s) const;

    vec3& operator+=(const vec3& v);
    vec3& operator-=(const vec3& v);
    vec3& operator*=(const float& s);

    float dot(const vec3& v) const;
    vec3 cross(const vec3& v) const;
    float norm() const;

    // vec3& normalize();  // never used, but implemented for completeness
    vec3 normalized() const;

    vec3 reflect(const vec3& n) const;
    // {
    //     return *this - n * 2.0f * this->dot(n) * (1/n.dot(n));  // Assumes non-normalized normal
    // }
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

// color class declaration
class col3{
    public:

    float r, g, b;
    col3() : r(0), g(0), b(0) {};
    col3(float r, float g, float b);

    col3 operator*(const float& s) const;   // scalar
    col3 operator*(const col3& c) const;    // component wise
    col3 operator/(const float& s) const;

    col3& operator+=(const col3& c);
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

// material class forward declaration
class material;

// hit record class declaration
class hit_record {
    public:

    float t;
    vec3 point;
    vec3 normal;

    std::shared_ptr<material> mat;
    bool front_face;    // External vs internal surface

    void set_face_normal(const ray& cast_ray, const vec3& out_normal);
};

// material class declaration
class material {
    public:

    virtual col3 get_albedo() const = 0; 
    virtual bool scatter(const ray& in_ray, const hit_record& rec, col3& attenuation, ray& scattered) const = 0;
};

// lambertian material class declaration
class lambertian : public material{
    public:

    col3 albedo;
    lambertian(const col3& albedo);

    col3 get_albedo() const override {return albedo; };
    bool scatter(const ray& in_ray, const hit_record& rec, col3& attenuation, ray& scattered) const override;

};

// metal material class declaration
class metal : public material{
    public:

    col3 albedo;
    metal(const col3& albedo);

    col3 get_albedo() const override {return albedo; };
    bool scatter(const ray& in_ray, const hit_record& rec, col3& attenuation, ray& scattered) const override;

};

// hittable class declaration
class hittable {
    public:

    virtual ~hittable() = default;
    virtual bool hit(const ray& ray, float t_min, float t_max, hit_record& rec) const = 0;
};

// sphere class declaration
class sphere : public hittable{
    public:

    vec3 center;
    float radius;
    std::shared_ptr<material> mat;

    sphere(const vec3& center, float radius, std::shared_ptr<material> mat);

    bool hit(const ray& ray, float t_min, float t_max, hit_record& rec) const override;
};

// hittable list class declaration
class hittable_list : public hittable{
    public:

    std::vector<std::shared_ptr<hittable>> objects;

    void add(std::shared_ptr<hittable> object);

    bool hit(const ray& ray, float t_min, float t_max, hit_record& rec) const override;
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

// directional light class declaration
class directional_light : public light {
    public:
    vec3 direction;
    col3 color;
    float radiance;

    directional_light(const vec3& direction, const col3& color, const float& radiance);
};

// random function declaration
float randf01();

// clamp function declaration
float clamp01(float x);

// random vector function declaration
vec3 rand_vec();

// Reinhard tone mapping function declaration
col3 reinhard_mapping(const col3& c);

// gamma correction function declaration
col3 gamma_correction(const col3& c);

// in shadow function declaration
bool in_shadow(const vec3& point, const vec3& out_normal, const vec3& light_dir, const hittable_list& scene);

// ray color function declaration
col3 ray_color(const ray& r, const hittable_list& scene, const directional_light& dir_light, int depth);

// gradient shader function declaration
inline col3 gradient_shader(const hittable_list& scene, image& img, const ray& cast_ray, hit_record& rec);

// masking shader function declaration
inline col3 masking_shader(const hittable_list& scene, image& img, const point_light& light, const ray& ray, hit_record& rec);

// lambertian shader function declaration
inline col3 lambertian_shader(const hittable_list& scene, image& img, const point_light& light, const ray& ray, hit_record& rec);

// thread worker function declaration
static inline void worker_rows(const pinhole_cam& cam, const hittable_list& scene, image& img, const directional_light& dir_light, int y0, int y1, int aa_N);

// rendering function declaration
void render(const pinhole_cam& cam, const hittable_list& scene, image& img, const directional_light& dir_light, int aa_N);