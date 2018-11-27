#ifndef SCENEOBJECT_H
#define SCENEOBJECT_H

#include <vector>
#include <memory>

#include "glm/glm.hpp"            // glm::vec*, mat*, and basic glm functions

#include "SceneObject.h"
#include "ScenePrimitive.h"
#include "lib/SceneData.h"

class SceneObject {
public:
    SceneObject();
    explicit SceneObject(const SceneObjectData &data);

    virtual ~SceneObject();

    virtual SceneObject& pushMatrix(const glm::mat4 &mat);

    virtual SceneObject& popMatrix();

    virtual SceneObject& resetMatrixStack();

    virtual SceneObject& setModelTransform(const glm::mat4 &transform);

    virtual SceneObject& assignChild(const std::shared_ptr<SceneObject> &cp);
    virtual SceneObject& assignChild(const std::shared_ptr<ScenePrimitive> &cp);

    virtual SceneObject& update();

protected:
    std::vector <glm::mat4> m_transformMatrices;
    glm::mat4 m_modelTransform;

    std::vector<std::weak_ptr<SceneObject>> m_object_ptrs;
    std::vector<std::weak_ptr<ScenePrimitive>> m_primitive_ptrs;
};


#endif //SCENEOBJECT_H
