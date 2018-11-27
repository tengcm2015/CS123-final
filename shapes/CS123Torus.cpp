#include "CS123Torus.h"

#include "glm/gtx/transform.hpp"  // glm::translate, scale, rotate

CS123Torus::CS123Torus(int slice, int ringSlice, float ringRadius, const glm::mat4 &transform)
: CS123Mesh(transform)
{
    slice      = std::max(3, slice);
    ringSlice  = std::max(3, ringSlice);
    ringRadius = std::max(0.0f, ringRadius);

    CS123Shape proto;
    proto.setDrawMode(DRAWMODE::STRIP);

    float theta = 2.0f*(float)M_PI/slice;
    float r = ringRadius / 100.0f;
    float R = 1.0f - r;

    for (int p = 0; p <= ringSlice; p++) {
        float phi = 2.0f*(float)M_PI/ringSlice * p;
        proto.pushVertex( glm::vec3((R + r*sin(phi))*cos(theta), (R + r*sin(phi))*sin(theta), r*cos(phi))
                        , glm::vec3(sin(phi)*cos(theta), sin(phi)*sin(theta), cos(phi)))
             .pushVertex(glm::vec3((R + r*sin(phi))*cos(0.0f)  , (R + r*sin(phi))*sin(0.0f) , r*cos(phi))
                        , glm::vec3(sin(phi)*cos(0.0f) , sin(phi)*sin(0.0f) , cos(phi)))
                        ;
    }

    for (int t = 0; t < slice; t++) {
        m_shapes.push_back(proto);
        m_shapes.back()
                .pushMatrix(glm::rotate(theta * t, glm::vec3(0.0f, 0.0f, 1.0f)))
                .pushMatrix(m_externalTransform);
    }
}

CS123Torus::~CS123Torus()
{
}
