#include "PhysicsObject.h"
#include "glm/gtx/transform.hpp"  // glm::translate, scale, rotate

namespace CS123 { namespace PHYSICS {

PhysicsObject::PhysicsObject()
: m_data()
, m_linearVelocity(0.0f)
, m_angularVelocity(0.0f)
, m_physicsTransform()
, m_targetTransform()
{
}

PhysicsObject::PhysicsObject(const PhysicsObjectData &data)
: m_data(data)
, m_linearVelocity(0.0f)
, m_angularVelocity(0.0f)
, m_physicsTransform()
, m_targetTransform()
{
    for (auto &t: data.transformations) {
        switch (t.type) {
            case CS123TransformationType::TRANSFORMATION_TRANSLATE:
                m_physicsTransform = glm::translate(t.translate) * m_physicsTransform;
                break;
            case CS123TransformationType::TRANSFORMATION_ROTATE:
                m_physicsTransform = glm::rotate(t.angle, t.rotate) * m_physicsTransform;
                break;
            case CS123TransformationType::TRANSFORMATION_SCALE:
                m_physicsTransform = glm::scale(t.scale) * m_physicsTransform;
                break;
            case CS123TransformationType::TRANSFORMATION_MATRIX:
                m_physicsTransform = t.matrix * m_physicsTransform;
        }
    }
}

PhysicsObject::~PhysicsObject()
{
}

}}
