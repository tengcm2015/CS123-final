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
    static const int MAX_SAMPLERS = 6;
    static const int MAX_PRIMITIVES = 50;

    RayMarchShader(const std::string &vertexSource, const std::string &fragmentSource);
    RayMarchShader(const std::string &vertexSource, const std::string &geometrySource, const std::string &fragmentSource);

    void setPrimitive(const std::shared_ptr<ScenePrimitive> &pp, int id);
    void clearPrimitive(int id);

    void setLight(const SceneLightData &light, int id);
    void clearLight(int id);

private:
    void setSampler(TextureRole role, const Texture2D &t);
};

}}


#endif //RAYMARCHSHADER_H
