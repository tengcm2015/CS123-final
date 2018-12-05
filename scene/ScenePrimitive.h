#ifndef SCENEPRIMITIVE_H
#define SCENEPRIMITIVE_H

#include <vector>
#include <memory>
#include <QImage>

#include "lib/BGRA.h"
#include "SceneData.h"
#include "gl/textures/Texture2D.h"

class CS123Primitive;

class ScenePrimitive {
public:
    explicit ScenePrimitive(const ScenePrimitiveData &data);

    virtual ~ScenePrimitive();

    PrimitiveType type()         const { return m_data.type; }
    std::string   meshfilePath() const { return m_data.meshfile; }
    SceneMaterial getMaterial()  const { return m_data.material; }

    bool textureUsed() const;
    CS123::GL::Texture2D &getTexture2D() const;
    BGRA sampleTexture(const glm::vec2 &uv, const glm::vec2 &repeatUV) const;

    virtual glm::mat4 getModelTransform() const { return m_modelTransform; }
    virtual ScenePrimitive& setModelTransform(const glm::mat4 &t) { m_modelTransform = t; return *this; }

    virtual ScenePrimitive& update(int p1 = 15, int p2 = 15, float p3 = 15.0);

    virtual ScenePrimitive& draw();

    bool castRay(const glm::vec4 &startPoint, const glm::vec4 &worldSpaceRay,
            float &t, glm::vec4 &normal, glm::vec2 &uv) const;

protected:
    int m_p1, m_p2; float m_p3;

    glm::mat4 m_modelTransform;
    ScenePrimitiveData m_data;

    std::string m_textureFileName;
    std::unique_ptr<CS123Primitive> m_primitive_ptr;
};

#endif // SCENEPRIMITIVE_H
