#include "CS123Cube.h"

#include "glm/gtx/transform.hpp"  // glm::translate, scale, rotate

#define PI static_cast<float>(M_PI)

CS123Cube::CS123Cube(int stack, const glm::mat4 &transform)
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

    // face z+
    for (auto &line: face_proto) {
        m_shapes.push_back(line);
        m_shapes.back()
                .pushMatrix(glm::translate(glm::vec3(0.0f, 0.0f, 0.5f)))
                .pushMatrix(m_externalTransform);
    }

    // face z-
    for (auto &line: face_proto) {
        m_shapes.push_back(line);
        m_shapes.back()
                .pushMatrix(glm::rotate(PI, glm::vec3(0, 1, 0)))
                .pushMatrix(glm::translate(glm::vec3(0.0f, 0.0f, -0.5f)))
                .pushMatrix(m_externalTransform);
    }

    // face y+
    for (auto &line: face_proto) {
        m_shapes.push_back(line);
        m_shapes.back()
                .pushMatrix(glm::rotate(-PI/2, glm::vec3(1, 0, 0)))
                .pushMatrix(glm::translate(glm::vec3(0.0f, 0.5f, 0.0f)))
                .pushMatrix(m_externalTransform);
    }

    // face y-
    for (auto &line: face_proto) {
        m_shapes.push_back(line);
        m_shapes.back()
                .pushMatrix(glm::rotate(PI/2, glm::vec3(1, 0, 0)))
                .pushMatrix(glm::translate(glm::vec3(0.0f, -0.5f, 0.0f)))
                .pushMatrix(m_externalTransform);
    }

    // face x+
    for (auto &line: face_proto) {
        m_shapes.push_back(line);
        m_shapes.back()
                .pushMatrix(glm::rotate(PI/2, glm::vec3(0, 1, 0)))
                .pushMatrix(glm::translate(glm::vec3(0.5f, 0.0f, 0.0f)))
                .pushMatrix(m_externalTransform);
    }

    // face x-
    for (auto &line: face_proto) {
        m_shapes.push_back(line);
        m_shapes.back()
                .pushMatrix(glm::rotate(-PI/2, glm::vec3(0, 1, 0)))
                .pushMatrix(glm::translate(glm::vec3(-0.5f, 0.0f, 0.0f)))
                .pushMatrix(m_externalTransform);
    }


}

CS123Cube::~CS123Cube()
{
}

bool CS123Cube::castRay(const glm::vec4 &p, const glm::vec4 &ray, float &min_t, glm::vec4 &normal, glm::vec2 &uv) {
    bool hit = false;
    min_t = std::numeric_limits<float>::infinity();

    // +x
    float t = (0.5f - p.x) / ray.x;
    auto itn/* intersection */ = p + t * ray;
    if (itn.y < 0.5 && itn.y > -0.5 && itn.z < 0.5 && itn.z > -0.5) {
        hit = true;
        min_t = t;
        normal = {1, 0, 0, 0};
        uv = {0.5 - itn.z, 0.5 + itn.y};
    }

    // -x
    t = (-0.5f - p.x) / ray.x;
    itn = p + t * ray;
    if (itn.y < 0.5 && itn.y > -0.5 && itn.z < 0.5 && itn.z > -0.5 && t < min_t) {
        hit = true;
        min_t = t;
        normal = {-1, 0, 0, 0};
        uv = {0.5 + itn.z, 0.5 + itn.y};
    }

    // +y
    t = (0.5f - p.y) / ray.y;
    itn = p + t * ray;
    if (itn.x < 0.5 && itn.x > -0.5 && itn.z < 0.5 && itn.z > -0.5 && t < min_t) {
        hit = true;
        min_t = t;
        normal = {0, 1, 0, 0};
        uv = {0.5 + itn.x, 0.5 - itn.z};
    }

    // -y
    t = (-0.5f - p.y) / ray.y;
    itn = p + t * ray;
    if (itn.x < 0.5 && itn.x > -0.5 && itn.z < 0.5 && itn.z > -0.5 && t < min_t) {
        hit = true;
        min_t = t;
        normal = {0, -1, 0, 0};
        uv = {0.5 + itn.x, 0.5 + itn.z};
    }

    // +z
    t = (0.5f - p.z) / ray.z;
    itn = p + t * ray;
    if (itn.y < 0.5 && itn.y > -0.5 && itn.x < 0.5 && itn.x > -0.5 && t < min_t) {
        hit = true;
        min_t = t;
        normal = {0, 0, 1, 0};
        uv = {0.5 + itn.x, 0.5 + itn.y};
    }

    // -z
    t = (-0.5f - p.z) / ray.z;
    itn = p + t * ray;
    if (itn.y < 0.5 && itn.y > -0.5 && itn.x < 0.5 && itn.x > -0.5 && t < min_t) {
        hit = true;
        min_t = t;
        normal = {0, 0, -1, 0};
        uv = {0.5 - itn.x, 0.5 + itn.y};
    }

    if (hit) {
        uv = glm::clamp(uv, 0.0f, 1.0f);
    }
    return hit;
}
