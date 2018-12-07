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

    glm::mat4 getTransformation() const { return m_physicsTransform; }

    glm::vec4 getPosition() const { return m_physicsTransform * glm::vec4(0,0,0,1); }

    PhysicsFlag     getFlag()     const { return m_data.flag; }
    GeometryType    getGeometry() const { return m_data.type; }
    PhysicsMaterial getMaterial() const { return m_data.material; }

    PhysicsObject& setFlag    (PhysicsFlag flag)  { m_data.flag = flag; return *this; }
    PhysicsObject& setGeomerty(GeometryType type) { m_data.type = type; return *this; }


    PhysicsObject& setModelTransform (const glm::mat4 &t) { m_physicsTransform = t; return *this; }

    // setModelTransform will teleport the object to the destination, which is unnatural
    // instead here we will make this object interact with other objects along the way
    PhysicsObject& setKinematicTarget(const glm::mat4 &t) { m_targetTransform = t;  return *this; }


    glm::vec3 getLinearVelocity () const { return m_linearVelocity; }
    glm::vec3 getAngularVelocity() const { return m_angularVelocity; }

    PhysicsObject& setLinearVelocity (glm::vec3 const &v) { m_linearVelocity = v;  return *this; }
    PhysicsObject& setAngularVelocity(glm::vec3 const &v) { m_angularVelocity = v; return *this; }

private:
    // PhysicsScene can access private members of PhysicsObject
    friend class PhysicsScene;

    PhysicsObjectData m_data;

    glm::vec3 m_linearVelocity;
    glm::vec3 m_angularVelocity;

    glm::mat4 m_physicsTransform;
    glm::mat4 m_targetTransform;
};

}}

#endif //PHYSICSOBJECT_H
