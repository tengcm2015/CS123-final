#ifndef CS123SHADER_H
#define CS123SHADER_H

#include "Shader.h"

class SceneMaterial;
class SceneLightData;

namespace CS123 { namespace GL {

class PhongShader : public Shader {
public:
    PhongShader(const std::string &vertexSource, const std::string &fragmentSource);
    PhongShader(const std::string &vertexSource, const std::string &geometrySource, const std::string &fragmentSource);

    void applyMaterial(const SceneMaterial &material);
    void setLight(const SceneLightData &light);
};

}}

#endif // CS123SHADER_H
