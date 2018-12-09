#ifndef RAYMARCHSHADER_H
#define RAYMARCHSHADER_H

#include <scene/SceneObject.h>
#include <array>
#include "Shader.h"

class SceneMaterial;
class SceneLightData;

namespace CS123 { namespace GL {

class RayMarchShader : public Shader {
public:
    static const int MAX_NUM_LIGHTS = 10;
    static const int MAX_SAMPLERS = 16;
    static const int MAX_PRIMITIVES = 50;

    RayMarchShader(const std::string &vertexSource, const std::string &fragmentSource);
    RayMarchShader(const std::string &vertexSource, const std::string &geometrySource, const std::string &fragmentSource);

    void insertPrimitive(const std::shared_ptr<ScenePrimitive> &pp);
    void deletePrimitive(const std::shared_ptr<ScenePrimitive> &primitive);
    void reloadPrimitives();

    void setLight(const SceneLightData &light);
    void clearLight(int id);

private:
    void loadPrimitive(int id);
    void clearPrimitive(int id);

    std::array<std::shared_ptr<ScenePrimitive>, MAX_PRIMITIVES> m_primitive_ptrs;
    std::array<std::shared_ptr<Texture2D>, MAX_SAMPLERS> m_texture2D_ptrs;
};

}}


#endif //RAYMARCHSHADER_H
