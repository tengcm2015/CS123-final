#include "PhysicsObject.h"
#include "glm/gtx/transform.hpp"  // glm::translate, scale, rotate

using namespace CS123::UTIL;

namespace CS123 { namespace PHYSICS {

PhysicsObject::PhysicsObject()
: m_modelTransform()
, m_physicsTransform()
, m_flag(PhysicsFlag::FLAG_STATIC)
{
}

PhysicsObject::PhysicsObject(const PhysicsObjectData &data)
: m_modelTransform()
, m_physicsTransform()
, m_flag(PhysicsFlag::FLAG_STATIC)
{
    for (auto &t: data.transformations) {
        switch (t.type) {
            case TransformationType::TRANSFORMATION_TRANSLATE:
                m_modelTransform = glm::translate(t.translate) * m_modelTransform;
                break;
            case TransformationType::TRANSFORMATION_ROTATE:
                m_modelTransform = glm::rotate(t.angle, t.rotate);
                break;
            case TransformationType::TRANSFORMATION_SCALE:
                m_modelTransform = glm::scale(t.scale);
                break;
            case TransformationType::TRANSFORMATION_MATRIX:
                m_modelTransform = t.matrix;
        }
    }
}

PhysicsObject::~PhysicsObject()
{
}

PhysicsObject& PhysicsObject::setModelTransform(const glm::mat4 &t) {
    m_modelTransform = t;
    return *this;
}

// setModelTransform will teleport the object to the destination, which is unnatural
// instead here we will make this object interact with other objects along the way
PhysicsObject &PhysicsObject::setKinematicTarget(const glm::mat4 &t) {
    m_targetTransform = t;
    return *this;
}


}}