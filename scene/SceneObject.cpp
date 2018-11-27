#include "SceneObject.h"
#include "lib/SceneData.h"

#include "glm/gtx/transform.hpp"  // glm::translate, scale, rotate

SceneObject::SceneObject()
: m_modelTransform(1.0f)
{
}

SceneObject::SceneObject(const SceneObjectData &data)
: m_modelTransform(1.0f)
{
    for (auto &t: data.transformations) {
        switch (t.type) {
            case TransformationType::TRANSFORMATION_TRANSLATE:
                this->pushMatrix(glm::translate(t.translate));
                break;
            case TransformationType::TRANSFORMATION_ROTATE:
                this->pushMatrix(glm::rotate(t.angle, t.rotate));
                break;
            case TransformationType::TRANSFORMATION_SCALE:
                this->pushMatrix(glm::scale(t.scale));
                break;
            case TransformationType::TRANSFORMATION_MATRIX:
                this->pushMatrix(t.matrix);
        }
    }
}

SceneObject::~SceneObject()
{
}

SceneObject& SceneObject::pushMatrix(const glm::mat4 &mat) {
    m_transformMatrices.push_back(mat);
    return *this;
}

SceneObject& SceneObject::popMatrix() {
    m_transformMatrices.pop_back();
    return *this;
}

SceneObject& SceneObject::resetMatrixStack() {
    m_transformMatrices.clear();
    return *this;
}

SceneObject& SceneObject::setModelTransform(const glm::mat4 &transform) {
    m_modelTransform = transform;
    return *this;
}

SceneObject& SceneObject::assignChild(const std::shared_ptr<SceneObject> &cp) {
    m_object_ptrs.push_back(cp);
    return *this;
}

SceneObject& SceneObject::assignChild(const std::shared_ptr<ScenePrimitive> &cp) {
    m_primitive_ptrs.push_back(cp);
    return *this;
}

SceneObject& SceneObject::update() {
    glm::mat4 transform(1.0f);
    for (auto &matrix: m_transformMatrices)
        transform = matrix * transform;
    transform = m_modelTransform * transform;

    for (auto &pp: m_primitive_ptrs) {
        if (auto sp = pp.lock())
            sp->setModelTransform(transform).update();
    }

    for (auto &op: m_object_ptrs) {
        if (auto sp = op.lock())
            sp->setModelTransform(transform).update();
    }

    return *this;
}

