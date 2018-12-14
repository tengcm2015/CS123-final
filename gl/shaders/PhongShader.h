#ifndef PHONGSHADER_H
#define PHONGSHADER_H

#include "Shader.h"

class SceneMaterial;
class SceneLightData;

namespace CS123 { namespace GL {

class PhongShader : public Shader {
public:
    static const int MAX_NUM_LIGHTS = 10;

    PhongShader(const std::string &vertexSource, const std::string &fragmentSource);
    PhongShader(const std::string &vertexSource, const std::string &geometrySource, const std::string &fragmentSource);

    void applyMaterial(const SceneMaterial &material);
    void setLight(const SceneLightData &light, int id);
    void clearLight(int id);
};

}}

#endif // PHONGSHADER_H
