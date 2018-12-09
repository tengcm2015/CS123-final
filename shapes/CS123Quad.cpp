#include "CS123Quad.h"

#include "glm/gtx/transform.hpp"  // glm::translate, scale, rotate

#define PI static_cast<float>(M_PI)

CS123Quad::CS123Quad(int stack, const glm::mat4 &transform)
: CS123Mesh(transform)
{
    stack = std::max(1, stack);

    std::vector<CS123Shape> face_proto;

    for (int y = 0; y < stack; ++y) {
        face_proto.emplace_back();
        CS123Shape &line = face_proto.back().setDrawMode(DRAWMODE::STRIP);

        for (int x = 0; x <= stack; ++x) {
            line.pushVertex( glm::vec3((float)x/stack, (float)(y+1)/stack, 0.0f)
                           , glm::vec3(0.0f, 0.0f, 1.0f)
                           , glm::vec2((float)x/stack, (float)(y+1)/stack))
                .pushVertex( glm::vec3((float)x/stack, (float)y/stack, 0.0f)
                           , glm::vec3(0.0f, 0.0f, 1.0f)
                           , glm::vec2((float)x/stack, (float)y/stack))
                           ;
        }

        line.pushMatrix(glm::translate(glm::vec3(-0.5f, -0.5f, 0.0f)));
    }

    for (auto &line: face_proto) {
        m_shapes.push_back(line);
        m_shapes.back()
                .pushMatrix(m_externalTransform);
    }

}

CS123Quad::~CS123Quad()
{
}

bool CS123Quad::castRay(const glm::vec4 &p, const glm::vec4 &ray, float &min_t, glm::vec4 &normal, glm::vec2 &uv) {
    bool hit = false;
    min_t = std::numeric_limits<float>::infinity();

    // +z
    float t = p.z / ray.z;
    auto itn/* intersection */ = p + t * ray;
    if (itn.y < 0.5 && itn.y > -0.5 && itn.x < 0.5 && itn.x > -0.5 && t < min_t) {
        hit = true;
        min_t = t;
        normal = {0, 0, 1, 0};
        uv = {0.5 + itn.x, 0.5 + itn.y};
    }

    if (hit) {
        uv = glm::clamp(uv, 0.0f, 1.0f);
    }
    return hit;
}
