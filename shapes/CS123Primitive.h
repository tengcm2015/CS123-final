#ifndef CS123PRIMITIVE_H
#define CS123PRIMITIVE_H

#include <vector>
#include "glm/glm.hpp"

class CS123Primitive {
public:
    explicit CS123Primitive(const glm::mat4 &t) : m_externalTransform(t) {}

    virtual ~CS123Primitive() = default;

    virtual CS123Primitive& draw() = 0;

protected:
    glm::mat4 m_externalTransform;
};

#endif // CS123PRIMITIVE_H
