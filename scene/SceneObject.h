#ifndef SCENEOBJECT_H
#define SCENEOBJECT_H

#include <vector>
#include <memory>

#include "glm/glm.hpp"            // glm::vec*, mat*, and basic glm functions
#include "physics/PhysicsObject.h"
#include "SceneObject.h"
#include "ScenePrimitive.h"
#include "SceneData.h"

class SceneObject {
public:
    SceneObject();
    explicit SceneObject(const SceneObjectData &data);

    virtual ~SceneObject();

    virtual SceneObject& pushMatrix(const glm::mat4 &mat);

    virtual SceneObject& popMatrix();

    virtual SceneObject& resetMatrixStack();

    virtual SceneObject& setModelTransform(const glm::mat4 &transform);

    virtual SceneObject& assignChild  (const std::shared_ptr<SceneObject> &cp);
    virtual SceneObject& assignChild  (const std::shared_ptr<ScenePrimitive> &cp);
    virtual SceneObject& assignPhysics(const std::shared_ptr<CS123::PHYSICS::PhysicsObject> &op);

    virtual std::shared_ptr<CS123::PHYSICS::PhysicsObject> getPhysicsObject() {
        return m_physicsObject_ptr.lock();
    }

    virtual std::shared_ptr<SceneObject> getChildObject(int index) {
        return m_object_ptrs.at(index).lock();
    }

    virtual std::shared_ptr<ScenePrimitive> getPrimitive(int index) {
        return m_primitive_ptrs.at(index).lock();
    }

    virtual SceneObject& update();

protected:
    std::vector <glm::mat4> m_transformMatrices;
    glm::mat4 m_modelTransform;

    std::weak_ptr<CS123::PHYSICS::PhysicsObject> m_physicsObject_ptr;

    std::vector<std::weak_ptr<SceneObject>> m_object_ptrs;
    std::vector<std::weak_ptr<ScenePrimitive>> m_primitive_ptrs;
};


#endif //SCENEOBJECT_H
