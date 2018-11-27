#ifndef CS123SHAPE_H
#define CS123SHAPE_H

#include <vector>

#include "glm/glm.hpp"            // glm::vec*, mat*, and basic glm functions
#include "gl/OpenGLShape.h"

enum class DRAWMODE {
    TRIANGLE = CS123::GL::VBO::GEOMETRY_LAYOUT::LAYOUT_TRIANGLES,
    STRIP = CS123::GL::VBO::GEOMETRY_LAYOUT::LAYOUT_TRIANGLE_STRIP,
    FAN = CS123::GL::VBO::GEOMETRY_LAYOUT::LAYOUT_TRIANGLE_FAN
};

struct CS123Vertex {
    explicit CS123Vertex (
        const glm::vec3 &p = {},
        const glm::vec3 &n = {},
        const glm::vec2 &uv = {}
    ) : pos(p), norm(n), uv(uv) {};

    glm::vec3 pos;
    glm::vec3 norm;
    glm::vec2 uv;
};

class CS123Shape {
public:
    CS123Shape();
    CS123Shape(const CS123Shape &that);
    CS123Shape& operator=(const CS123Shape &that);
    CS123Shape(CS123Shape &&that);
    CS123Shape& operator=(CS123Shape &&that);
    ~CS123Shape();

    CS123Shape& setDrawMode(DRAWMODE m);

    CS123Shape& pushMatrix(glm::mat4 mat);
    CS123Shape& popMatrix();

    CS123Shape& pushVertex( const CS123Vertex &v );
    CS123Shape& pushVertex( const glm::vec3 &pos  = {}
                          , const glm::vec3 &norm = {}
                          , const glm::vec2 &uv   = {}
                          );

    CS123Shape& popVertex();

    CS123Shape& draw();

private:
    bool m_modified;

    DRAWMODE m_drawMode;

    std::vector<struct CS123Vertex> m_vertices;
    std::vector<glm::mat4> m_transformMatrices;
    std::unique_ptr<CS123::GL::OpenGLShape> m_openGLShape;
};

#endif // CS123SHAPE_H
