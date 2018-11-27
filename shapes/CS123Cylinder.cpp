#include "CS123Cylinder.h"

#include "glm/glm.hpp"            // glm::vec*, mat*, and basic glm functions
#include "glm/gtx/transform.hpp"  // glm::translate, scale, rotate

#define PI static_cast<float>(M_PI)

CS123Cylinder::CS123Cylinder(int stack, int slice, const glm::mat4 &transform)
: CS123Mesh(transform)
{
    stack = std::max(1, stack);
    slice = std::max(3, slice);

    float theta = 2.0f*PI/slice;

    std::vector<CS123Shape> side_proto;
    for (int s = 0; s < slice; ++s) {
        side_proto.emplace_back();
        CS123Shape &strip = side_proto.back().setDrawMode(DRAWMODE::STRIP);

        for (int y = 0; y <= stack; ++y) {
            float r = 0.5;
            strip.pushVertex( glm::vec3(r*cos((s+1)*theta)  , (float)y/stack, r*sin((s+1)*theta))
                            , glm::vec3(cos((s+1)*theta)    , 0.0f          , sin((s+1)*theta)  )
                            , glm::vec2(1-(float)(s+1)/slice, (float)y/stack                    )
                            )
                 .pushVertex( glm::vec3(r*cos(s*theta)      , (float)y/stack, r*sin(s*theta))
                            , glm::vec3(cos(s*theta)        , 0.0f          , sin(s*theta)  )
                            , glm::vec2(1-(float)s/slice    , (float)y/stack                )
                            );
        }

    }

    for (auto &strip: side_proto) {
        m_shapes.push_back(strip);
        m_shapes.back()
                .pushMatrix(glm::translate(glm::vec3(0.0f, -0.5f, 0.0f)))
                .pushMatrix(m_externalTransform);
    }


    std::vector<CS123Shape> &cap_proto = side_proto;
    CS123Mesh::createUnitPolygon(cap_proto, slice, stack);

    // cap y+
    for (auto &strip: cap_proto) {
        m_shapes.push_back(strip);
        m_shapes.back()
                .pushMatrix(glm::translate(glm::vec3(0.0f, 0.5f, 0.0f)))
                .pushMatrix(m_externalTransform);
    }

    // cap y-
    for (auto &strip: cap_proto) {
        m_shapes.push_back(strip);
        m_shapes.back()
                .pushMatrix(glm::rotate(PI, glm::vec3(1, 0, 0)))
                .pushMatrix(glm::translate(glm::vec3(0.0f, -0.5f, 0.0f)))
                .pushMatrix(m_externalTransform);
    }
}

CS123Cylinder::~CS123Cylinder()
{
}

static inline glm::vec2 getSideUV(const glm::vec4 &p) {
    float u = p.z > 0 ? 1 - acos(p.x/0.5f) / (2*PI) : acos(p.x/0.5f) / (2*PI);
    return {u, 0.5 + p.y};
}

bool CS123Cylinder::castRay(const glm::vec4 &p, const glm::vec4 &ray, float &min_t, glm::vec4 &normal, glm::vec2 &uv) {
    bool hit = false;
    min_t = std::numeric_limits<float>::infinity();

    // side
    float A = ray.x * ray.x + ray.z * ray.z;
    float B = 2 * (p.x * ray.x + p.z * ray.z);
    float C = p.x * p.x + p.z * p.z - 1.f / 4;
    float dis = B * B - 4 * A * C;

    if (dis > 0) {
        float t1 = (-B - std::sqrt(dis)) / (2 * A);
        float t2 = (-B + std::sqrt(dis)) / (2 * A);

        auto itn1/* intersection */ = p + t1 * ray;
        auto itn2/* intersection */ = p + t2 * ray;

        if (itn1.y < 0.5 && itn1.y > -0.5) {
            hit = true;
            min_t = t1;
            normal = {itn1.x, 0, itn1.z, 0};
            uv = getSideUV(itn1);

        } else if (itn2.y < 0.5 && itn2.y > -0.5) {
            hit = true;
            min_t = t2;
            normal = {itn2.x, 0, itn2.z, 0};
            uv = getSideUV(itn2);
        }
    }

    // +y cap
    float t = (0.5f - p.y) / ray.y;
    auto itn/* intersection */ = p + t * ray;
    if (itn.x * itn.x + itn.z * itn.z < 0.25 && t < min_t) {
        hit = true;
        min_t = t;
        normal = {0, 1, 0, 0};
        uv = {0.5f + itn.x, 0.5f - itn.z};
    }

    // -y cap
    t = (-0.5f - p.y) / ray.y;
    itn/* intersection */ = p + t * ray;
    if (itn.x * itn.x + itn.z * itn.z < 0.25 && t < min_t) {
        hit = true;
        min_t = t;
        normal = {0, -1, 0, 0};
        uv = {0.5f + itn.x, 0.5f + itn.z};
    }

    if (hit) {
        normal = glm::normalize(normal);
        uv = glm::clamp(uv, 0.0f, 1.0f);
    }
    return hit;
}
