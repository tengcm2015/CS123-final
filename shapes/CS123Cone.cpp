#include "CS123Cone.h"

#include "glm/gtx/transform.hpp"  // glm::translate, scale, rotate

#define PI static_cast<float>(M_PI)

CS123Cone::CS123Cone(int stack, int slice, const glm::mat4 &transform)
: CS123Mesh(transform)
{
    stack = std::max(1, stack);
    slice = std::max(3, slice);

    float theta = 2.0f*PI/slice;

    std::vector<CS123Shape> side_proto;

    for (int s = 0; s < slice; ++s) {
        side_proto.emplace_back();
        CS123Shape &strip = side_proto.back().setDrawMode(DRAWMODE::STRIP);

        for (int y = 0; y < stack; ++y) {
            float r = 0.5f - 0.5f / (float) stack * y;
            strip.pushVertex( glm::vec3(r * cos((s+1)*theta)   , (float)y/stack, r * sin((s+1)*theta)   )
                            , glm::vec3(2.0f * cos((s+1)*theta), 1.0f          , 2.0f * sin((s+1)*theta))
                            , glm::vec2(1-(float)(s+1)/slice   , (float)y/stack                         )
                            )
                 .pushVertex( glm::vec3(r * cos(s*theta)       , (float)y/stack, r * sin(s*theta)       )
                            , glm::vec3(2.0f * cos(s*theta)    , 1.0f          , 2.0f * sin(s*theta)    )
                            , glm::vec2(1-(float)s/slice       , (float)y/stack                         )
                    );
        }

        strip.pushVertex( glm::vec3(0.0f                      , 1.0f, 0.0f                      )
                        , glm::vec3(2.0f * cos((s+0.5f)*theta), 1.0f, 2.0f * sin((s+0.5f)*theta))
                        , glm::vec2(1-(s+0.5f)/slice          , 1.0f                            )
                        );
    }

    for (auto &strip: side_proto) {
        m_shapes.push_back(strip);
        m_shapes.back()
                .pushMatrix(glm::translate(glm::vec3(0.0f, -0.5f, 0.0f)))
                .pushMatrix(m_externalTransform)
                ;
    }


    std::vector<CS123Shape> &bottom_proto = side_proto;
    CS123Mesh::createUnitPolygon(bottom_proto, slice, stack);

    for (auto &strip: bottom_proto) {
        m_shapes.push_back(strip);
        m_shapes.back()
                .pushMatrix(glm::rotate(PI, glm::vec3(1, 0, 0)))
                .pushMatrix(glm::translate(glm::vec3(0.0f, -0.5f, 0.0f)))
                .pushMatrix(m_externalTransform);
    }
}

CS123Cone::~CS123Cone()
{
}

static inline glm::vec2 getSideUV(const glm::vec4 &p) {
    float r = 0.5f * (0.5f - p.y);
    float u = p.z > 0 ? 1 - acos(p.x/r) / (2*PI) : acos(p.x/r) / (2*PI);
    return {u, 0.5 + p.y};
}

bool CS123Cone::castRay(const glm::vec4 &p, const glm::vec4 &ray, float &min_t, glm::vec4 &normal, glm::vec2 &uv) {
    bool hit = false;
    min_t = std::numeric_limits<float>::infinity();

    // side
    float A = ray.x * ray.x + ray.z * ray.z - ray.y * ray.y / 4;
    float B = 2 * p.x * ray.x + 2 * p.z * ray.z + (1 - 2 * p.y) * ray.y / 4;
    float C = p.x * p.x + p.z * p.z - p.y * p.y / 4 + p.y / 4 - 1.f / 16;
    float dis = B * B - 4 * A * C;

    if (dis < 0)
        return false; // don't even need to check for cap

    if (dis == 0) {
        float t = -B / (2 * A);
        auto itn/* intersection */ = p + t * ray;
        if (itn.y < 0.5 && itn.y > -0.5) {
            if (itn.y * itn.y / (itn.x * itn.x + itn.z * itn.z) == 4) {
                hit = true;
                min_t = t;
                normal = {itn.x, (0.5 - itn.y) / 4, itn.z, 0};
                uv = getSideUV(itn);
            }
        }

    } else {
        float t1 = (-B - std::sqrt(dis)) / (2 * A);
        float t2 = (-B + std::sqrt(dis)) / (2 * A);

        auto itn1/* intersection */ = p + t1 * ray;
        auto itn2/* intersection */ = p + t2 * ray;

        if (itn1.y < 0.5 && itn1.y > -0.5) {
            hit = true;
            min_t = t1;
            normal = {itn1.x, (0.5 - itn1.y) / 4, itn1.z, 0};
            uv = getSideUV(itn1);

        } else if (itn2.y < 0.5 && itn2.y > -0.5) {
            hit = true;
            min_t = t2;
            normal = {itn2.x, (0.5 - itn2.y) / 4, itn2.z, 0};
            uv = getSideUV(itn2);
        }
    }

    // bottom
    float t = (-0.5f - p.y) / ray.y;
    auto itn/* intersection */ = p + t * ray;
    if (itn.x * itn.x + itn.z * itn.z < 0.25 && t < min_t) {
        hit = true;
        min_t = t;
        normal = {0, -1, 0, 0};
        uv = {itn.x + 0.5f, itn.z + 0.5f};
    }

    if (hit) {
        normal = glm::normalize(normal);
        uv = glm::clamp(uv, 0.0f, 1.0f);
    }
    return hit;
}
