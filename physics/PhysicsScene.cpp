#include "PhysicsScene.h"
#include "PhysicsObject.h"

#include "glm/gtx/transform.hpp"  // glm::translate, scale, rotate

namespace CS123 { namespace PHYSICS {

PhysicsScene::PhysicsScene()
{
}

PhysicsScene::~PhysicsScene()
{
}

void PhysicsScene::simulate(int msecLapsed) {

    // TODO: update objects' translation matrices based on physics

    for (auto &op: m_object_ptrs) {
       if (op->getPosition().y < -5)
            op->m_physicsTransform = glm::translate(glm::vec3(0, 2, 0));
        else
            op->m_physicsTransform = glm::translate(glm::vec3(0, -0.01f * msecLapsed, 0)) * op->m_physicsTransform;
    }
}

std::shared_ptr<PhysicsObject> PhysicsScene::createObject(const PhysicsObjectData &data) {
    auto obj_ptr = std::make_shared<PhysicsObject>(data);
    m_object_ptrs.push_back(obj_ptr);
    return obj_ptr;
}

}}
