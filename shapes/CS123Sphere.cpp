#include "CS123Sphere.h"

#include "glm/gtx/transform.hpp"  // glm::translate, scale, rotate

#define PI static_cast<float>(M_PI)

CS123Sphere::CS123Sphere(int stack, int slice, const glm::mat4 &transform)
: CS123Mesh(transform)
{
    stack = std::max(2, stack);
    slice = std::max(3, slice);

    float theta = 2.0f*PI/slice;

    std::vector<CS123Shape> proto;
    for (int s = 0; s < slice; ++s) {
        proto.emplace_back();
        CS123Shape &strip = proto.back().setDrawMode(DRAWMODE::STRIP);

        strip.pushVertex(glm::vec3(0.0f, 0.5f, 0.0f), glm::vec3(0, 1, 0), glm::vec2(1-(s+0.5f)/slice, 1));
        for (int p = 1; p < stack; ++p) {
            float phi = PI/stack * p;
            strip.pushVertex( glm::vec3(0.5f*sin(phi)*cos((s+1)*theta), 0.5f*cos(phi), 0.5f*sin(phi)*sin((s+1)*theta))
                            , glm::vec3(sin(phi)*cos((s+1)*theta)     , cos(phi)     , sin(phi)*sin((s+1)*theta)     )
                            , glm::vec2(1-(float)(s+1)/slice          , 1 - (float)p/stack                           )
                            )
                 .pushVertex( glm::vec3(0.5f*sin(phi)*cos(s*theta)    , 0.5f*cos(phi), 0.5f*sin(phi)*sin(s*theta))
                            , glm::vec3(sin(phi)*cos(s*theta)         , cos(phi)     , sin(phi)*sin(s*theta)     )
                            , glm::vec2(1-(float)s/slice              , 1 - (float)p/stack)                      )
                            ;
        }
        strip.pushVertex(glm::vec3(0.0f, -0.5f, 0.0f), glm::vec3(0, -1, 0), glm::vec2(1-(s+0.5f)/slice, 0));
    }

    for (auto &strip: proto) {
        m_shapes.push_back(strip);
        m_shapes.back()
                .pushMatrix(m_externalTransform);
    }
}

CS123Sphere::~CS123Sphere()
{
}

static inline glm::vec2 getUV(const glm::vec4 &p) {
    float u;
    if (p.x > 0) {
        u = atan(-p.z/p.x) / (2*PI);
        u = u < 0 ? 1 + u : u;

    } else if (p.x < 0) {
        u = 0.5f + atan(-p.z/p.x) / (2*PI);

    } else {
        u = p.z > 0 ? 0.75f : 0.25f;
    }
    return {u, 0.5 + asin(p.y/0.5) / PI};
}

bool CS123Sphere::castRay(const glm::vec4 &p, const glm::vec4 &ray, float &min_t, glm::vec4 &normal, glm::vec2 &uv) {
    bool hit = false;
    min_t = std::numeric_limits<float>::infinity();

    float A = ray.x * ray.x + ray.y * ray.y + ray.z * ray.z;
    float B = 2 * (p.x * ray.x + p.y * ray.y + p.z * ray.z);
    float C = p.x * p.x + p.y * p.y + p.z * p.z - 1.f / 4;
    float dis = B * B - 4 * A * C;

    if (dis <= 0)
        return false;

    float t1 = (-B - std::sqrt(dis)) / (2 * A);
    float t2 = (-B + std::sqrt(dis)) / (2 * A);

    auto itn1/* intersection */ = p + t1 * ray;
    auto itn2/* intersection */ = p + t2 * ray;

    if (itn1.y < 0.5 && itn1.y > -0.5) {
        hit = true;
        min_t = t1;
        normal = {itn1.x, itn1.y, itn1.z, 0};
        uv = getUV(itn1);

    } else if (itn2.y < 0.5 && itn2.y > -0.5) {
        hit = true;
        min_t = t2;
        normal = {itn2.x, itn2.y, itn2.z, 0};
        uv = getUV(itn2);
    }

    if (hit) {
        normal = glm::normalize(normal);
        uv = glm::clamp(uv, 0.0f, 1.0f);
    }
    return hit;
}
