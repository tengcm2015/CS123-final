#ifndef CS123SHADER_H
#define CS123SHADER_H

#include "Shader.h"

class SceneMaterial;
class SceneLightData;

namespace CS123 { namespace GL {

class CS123Shader : public Shader {
public:
    CS123Shader(const std::string &vertexSource, const std::string &fragmentSource);
    CS123Shader(const std::string &vertexSource, const std::string &geometrySource, const std::string &fragmentSource);

    void applyMaterial(const SceneMaterial &material);
    void setLight(const SceneLightData &light);
};

}}

#endif // CS123SHADER_H
