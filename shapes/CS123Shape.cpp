#include "CS123Shape.h"

#include "glm/glm.hpp"            // glm::vec*, mat*, and basic glm functions
//#include "glm/gtx/transform.hpp"  // glm::translate, scale, rotate
//#include "glm/gtc/type_ptr.hpp"   // glm::value_ptr
#include "gl/shaders/ShaderAttribLocations.h"

using namespace CS123::GL;

CS123Shape::CS123Shape()
: m_modified(true)
, m_drawMode(DRAWMODE::TRIANGLE)
, m_openGLShape(std::make_unique<OpenGLShape>())
{
}

CS123Shape::CS123Shape(const CS123Shape &that)
: m_modified(true)
, m_drawMode(that.m_drawMode)
, m_vertices(that.m_vertices)
, m_transformMatrices(that.m_transformMatrices)
, m_openGLShape(std::make_unique<OpenGLShape>())
{
}

CS123Shape& CS123Shape::operator=(const CS123Shape &that) {
    // check for self-assignment
    if (&that == this)
        return *this;

    m_modified = true;
    m_drawMode = that.m_drawMode;
    m_vertices = that.m_vertices;
    m_transformMatrices = that.m_transformMatrices;
    m_openGLShape = std::make_unique<OpenGLShape>();

    return *this;
}

CS123Shape::CS123Shape(CS123Shape &&that)
: m_modified(that.m_modified)
, m_drawMode(that.m_drawMode)
, m_vertices(std::move(that.m_vertices))
, m_transformMatrices(std::move(that.m_transformMatrices))
, m_openGLShape(std::move(that.m_openGLShape))
{
}

CS123Shape& CS123Shape::operator=(CS123Shape &&that) {
    // check for self-assignment
    if (&that == this)
        return *this;

    this->~CS123Shape();

    m_modified = that.m_modified;
    m_drawMode = that.m_drawMode;
    m_vertices = std::move(that.m_vertices);
    m_transformMatrices = std::move(that.m_transformMatrices);
    m_openGLShape = std::move(that.m_openGLShape);

    return *this;
}

CS123Shape::~CS123Shape()
{
}

CS123Shape& CS123Shape::setDrawMode(DRAWMODE m) {
    m_modified = true;
    m_drawMode = m;
    return *this;
}


CS123Shape& CS123Shape::pushMatrix(glm::mat4 mat) {
    m_modified = true;
    m_transformMatrices.push_back(mat);
    return *this;
}

CS123Shape& CS123Shape::popMatrix() {
    m_modified = true;
    m_transformMatrices.pop_back();
    return *this;
}


CS123Shape& CS123Shape::pushVertex(const CS123Vertex &v) {
    m_vertices.push_back(v);
    return *this;
}

CS123Shape& CS123Shape::pushVertex(const glm::vec3 &pos, const glm::vec3 &norm, const glm::vec2 &uv) {
    m_modified = true;
    m_vertices.emplace_back(pos, norm, uv);
    return *this;
}

CS123Shape& CS123Shape::popVertex() {
    m_modified = true;
    m_vertices.pop_back();
    return *this;
}


CS123Shape& CS123Shape::draw() {
    if (m_modified) {
        glm::mat4 transform(1.0f);
        for (auto &matrix: m_transformMatrices)
            transform = matrix * transform;

        std::vector<GLfloat> coordinates;
        for (auto &v: m_vertices) {
            glm::vec4 vertex = transform * glm::vec4(v.pos, 1.0f);
            coordinates.push_back(vertex.x);
            coordinates.push_back(vertex.y);
            coordinates.push_back(vertex.z);
            // leave w alone
            glm::vec4 normal = glm::normalize(transform * glm::vec4(v.norm, 0.0f));
            coordinates.push_back(normal.x);
            coordinates.push_back(normal.y);
            coordinates.push_back(normal.z);

            coordinates.push_back(v.uv.x);
            coordinates.push_back(v.uv.y);
        }

        m_openGLShape->setVertexData(coordinates.data(), 8 * m_vertices.size(), (VBO::GEOMETRY_LAYOUT)m_drawMode, m_vertices.size());
        m_openGLShape->setAttribute(ShaderAttrib::POSITION , 3, 0*sizeof(float), VBOAttribMarker::DATA_TYPE::FLOAT, false);
        m_openGLShape->setAttribute(ShaderAttrib::NORMAL   , 3, 3*sizeof(float), VBOAttribMarker::DATA_TYPE::FLOAT, false);
        m_openGLShape->setAttribute(ShaderAttrib::TEXCOORD0, 2, 6*sizeof(float), VBOAttribMarker::DATA_TYPE::FLOAT, false);
        m_openGLShape->buildVAO();

        m_modified = false;
    }

    m_openGLShape->draw();
    return *this;
}
