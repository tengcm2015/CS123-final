#ifndef PHYSICSOBJECT_H
#define PHYSICSOBJECT_H

#include "PhysicsData.h"

namespace CS123 { namespace PHYSICS {

class PhysicsScene;

class PhysicsObject {
public:
    PhysicsObject();
    PhysicsObject(const PhysicsObjectData &data);

    virtual ~PhysicsObject();

    glm::mat4 getTransformation() const { return m_physicsTransform * m_modelTransform; }

    glm::vec4 getPosition() const { return getTransformation() * glm::vec4(0,0,0,1); }

    PhysicsObject& setModelTransform(const glm::mat4 &t);
    PhysicsObject& setKinematicTarget(const glm::mat4 &t);

    PhysicsObject& setFlag(PhysicsFlag flag) { m_flag = flag; return *this; }

    glm::vec3 getLinearVelocity() { return m_linearVelocity; }
    glm::vec3 getAngularVelocity() { return m_angularVelocity; }

    PhysicsObject& setLinearVelocity(glm::vec3 &v) { m_linearVelocity = v; return *this; }
    PhysicsObject& setAngularVelocity(glm::vec3 &v) { m_angularVelocity = v; return *this; }

private:
    // PhysicsScene can access private members of PhysicsObject
    friend class PhysicsScene;

    PhysicsFlag m_flag;

    PhysicsObjectData m_data;
    PhysicsMaterial m_material;

    glm::vec3 m_linearVelocity;
    glm::vec3 m_angularVelocity;

    glm::mat4 m_modelTransform;
    glm::mat4 m_physicsTransform;

    glm::mat4 m_targetTransform;
};

}}

#endif //PHYSICSOBJECT_H
