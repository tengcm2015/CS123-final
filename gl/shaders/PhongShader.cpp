#include "PhongShader.h"

#include "scene/SceneData.h"
#include "gl/GLDebug.h"

namespace CS123 { namespace GL {


PhongShader::PhongShader(const std::string &vertexSource, const std::string &fragmentSource)
: Shader(vertexSource, fragmentSource)
{
}

PhongShader::PhongShader(const std::string &vertexSource, const std::string &geometrySource, const std::string &fragmentSource)
: Shader(vertexSource, geometrySource, fragmentSource)
{
}

glm::vec3 toGLMVec3(const RGBAf &c) {
    return glm::vec3(c.r, c.g, c.b);
}

void PhongShader::applyMaterial(const SceneMaterial &material) {
    setUniform("ambient_color", toGLMVec3(material.cAmbient));
    setUniform("diffuse_color", toGLMVec3(material.cDiffuse));
    setUniform("specular_color", toGLMVec3(material.cSpecular));
    setUniform("shininess", material.shininess);
}

void PhongShader::setLight(const SceneLightData &light, int id) {
    if (id < 0 || id >= MAX_NUM_LIGHTS)
        return;

    bool ignoreLight = false;

    GLint lightType;
    std::string name;
    glm::vec3 ndir;
    switch(light.type) {
        case LightType::LIGHT_POINT:
            lightType = 0;
            name = "lightPositions";
            setUniformArrayByIndex(name, light.pos.xyz(), id);
            //if (!settings.usePointLights) ignoreLight = true;
            break;
        case LightType::LIGHT_DIRECTIONAL:
            lightType = 1;
            ndir = glm::normalize(light.dir.xyz());
            name = "lightDirections";
            setUniformArrayByIndex(name, ndir, id);
            //if (!settings.useDirectionalLights) ignoreLight = true;
            break;
        default:
            lightType = 0;
            ignoreLight = true; // Light type not supported
            break;
    }

    RGBAf color = light.color;
    if (ignoreLight) color.r = color.g = color.b = 0;

    setUniformArrayByIndex("lightTypes", lightType, id);
    setUniformArrayByIndex("lightColors", glm::vec3(color.r, color.g, color.b), id);
//    setUniformArrayByIndex("lightAttenuations", light.function, id);
}

void PhongShader::clearLight(int id) {
    if (id < 0 || id >= MAX_NUM_LIGHTS)
        return;
    setUniformArrayByIndex("lightColors", glm::vec3(0.0f), id);
}

}}
