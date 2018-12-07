#include "FullScreenQuad.h"

#include <vector>

#include "datatype/VBO.h"
#include "datatype/VBOAttribMarker.h"
#include "datatype/VAO.h"
#include "shaders/ShaderAttribLocations.h"

namespace CS123 { namespace GL {

FullScreenQuad::FullScreenQuad()
{
    float data[] = { -1.0f,  1.0f,   0.0f, 1.0f,
                     -1.0f, -1.0f,   0.0f, 0.0f,
                      1.0f,  1.0f,   1.0f, 1.0f,
                      1.0f, -1.0f,   1.0f, 0.0f };

    std::vector<CS123::GL::VBOAttribMarker> attribs;
    attribs.push_back(VBOAttribMarker(ShaderAttrib::POSITION, 2, 0));
    attribs.push_back(VBOAttribMarker(ShaderAttrib::TEXCOORD0, 2, 2*sizeof(float)));
    VBO vbo(data, sizeof(data), attribs, VBO::GEOMETRY_LAYOUT::LAYOUT_TRIANGLE_STRIP);
    m_vao = std::make_unique<VAO>(vbo, 4);
}

void FullScreenQuad::draw() {
    m_vao->bind();
    m_vao->draw();
    m_vao->unbind();
}

}}