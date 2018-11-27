#include "CS123Mesh.h"

#define PI static_cast<float>(M_PI)

CS123Mesh::CS123Mesh(const glm::mat4 &transform)
: CS123Primitive(transform)
{
}

CS123Mesh::~CS123Mesh()
{
}

#include <iostream>

CS123Primitive& CS123Mesh::draw() {
    for (auto &s: m_shapes)
        s.draw();
    return *this;
}

void CS123Mesh::createUnitPolygon(std::vector<CS123Shape> &shapes, int sides, int stack) {
    float theta = 2.0f*PI/sides;

    shapes.clear();
    for (int s = 0; s < sides; ++s) {
        shapes.emplace_back();
        CS123Shape &triangle = shapes.back().setDrawMode(DRAWMODE::STRIP);

        triangle.pushVertex(glm::vec3(0), glm::vec3(0, 1, 0), glm::vec2(0.5f, 0.5f));

        for (int z = 1; z <= stack; ++z) {
            float r = 0.5 * (float)z/stack;
            triangle.pushVertex( glm::vec3(r*cos((s+1)*theta)       , 0.0f, r*sin((s+1)*theta)  )
                               , glm::vec3(0.0f                     , 1.0f, 0.0f                )
                               , glm::vec2(r*cos((s+1)*theta) + 0.5f, -r*sin((s+1)*theta) + 0.5f)
                               )
                    .pushVertex( glm::vec3(r*cos(s*theta)           , 0.0f, r*sin(s*theta)      )
                               , glm::vec3(0.0f                     , 1.0f, 0.0f                )
                               , glm::vec2(r*cos(s*theta) + 0.5f    , -r*sin(s*theta) + 0.5f    )
                               );
        }
    }
}
