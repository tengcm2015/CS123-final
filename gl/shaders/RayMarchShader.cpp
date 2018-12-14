#include "RayMarchShader.h"

#include "lib/CS123Utils.h"
#include "scene/SceneData.h"
#include "gl/GLDebug.h"

#include <sstream>

namespace CS123 { namespace GL {

RayMarchShader::RayMarchShader(const std::string &vertexSource, const std::string &fragmentSource)
: Shader(vertexSource, fragmentSource)
{
}

RayMarchShader::RayMarchShader(const std::string &vertexSource, const std::string &geometrySource, const std::string &fragmentSource)
: Shader(vertexSource, geometrySource, fragmentSource)
{
}

void RayMarchShader::setPrimitive(const std::shared_ptr<ScenePrimitive> &pp, int id) {
    if (id < 0 || id >= MAX_PRIMITIVES)
        return;

    setUniformArrayByIndex("primitiveType"     , UTILS::as_integer(pp->type()), id);
    setUniformArrayByIndex("primitiveTransform", pp->getModelTransform(), id);

    auto m = pp->getMaterial();

    setUniformArrayByIndex("primitiveAmbient"    , m.cAmbient.xyz()    , id);
    setUniformArrayByIndex("primitiveDiffuse"    , m.cDiffuse.xyz()    , id);
    setUniformArrayByIndex("primitiveReflective" , m.cReflective.xyz() , id);
    setUniformArrayByIndex("primitiveSpecular"   , m.cSpecular.xyz()   , id);
    setUniformArrayByIndex("primitiveTransparent", m.cTransparent.xyz(), id);
    setUniformArrayByIndex("primitiveEmissive"   , m.cEmissive.xyz()   , id);

    setUniformArrayByIndex("primitiveShininess"  , m.shininess, id);
    setUniformArrayByIndex("primitiveIOR"        , m.ior      , id);

    if (m.textureMap.isUsed) {
        int texID = UTILS::as_integer(m.textureMap.role);
        setUniformArrayByIndex("primitiveTexID", texID, id);
        setUniformArrayByIndex("primitiveBlend", m.blend, id);
        this->setSampler(m.textureMap.role, pp->getTexture2D());

    } else {
        setUniformArrayByIndex("primitiveTexID", UTILS::as_integer(TextureRole::NO_TEX), id);
    }

    if (m.bumpMap.isUsed) {
        int texID = UTILS::as_integer(m.bumpMap.role);
        setUniformArrayByIndex("primitiveBumpID", texID, id);
        this->setSampler(m.textureMap.role, pp->getTexture2D());

    } else {
        setUniformArrayByIndex("primitiveBumpID", UTILS::as_integer(TextureRole::NO_TEX), id);
    }
}


void RayMarchShader::clearPrimitive(int id) {
    if (id < 0 || id >= MAX_PRIMITIVES)
        return;

    setUniformArrayByIndex("primitiveType", UTILS::as_integer(PrimitiveType::PRIMITIVE_NONE), id);
    setUniformArrayByIndex("primitiveTexID"  , -1, id);
    setUniformArrayByIndex("primitiveBumpID" , -1, id);
}


void RayMarchShader::setLight(const SceneLightData &light, int id) {
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
//    setUniformArrayByIndex("lightAttenuations", light.function, light.id);
}

void RayMarchShader::clearLight(int id) {
    if (id < 0 || id >= MAX_NUM_LIGHTS)
        return;
    setUniformArrayByIndex("lightColors", glm::vec3(0.0f), id);
}


void RayMarchShader::setSampler(TextureRole role, const Texture2D &t) {
    std::string uniformName;
    switch (role) {
    case TextureRole::SPHERE_SMOOTH_TEX:
        uniformName = "sphereSmoothTex";
        break;

    case TextureRole::SPHERE_PATTERNED_TEX:
        uniformName = "spherePatternedTex";
        break;

    case TextureRole::SPHERE_PATTERNED_BUMP:
        uniformName = "spherePatternedBump";
        break;

    case TextureRole::QUAD_SMOOTH_TEX:
        uniformName = "quadSmoothTex";
        break;

    case TextureRole::QUAD_PATTERNED_TEX:
        uniformName = "quadPatternedTex";
        break;

    case TextureRole::QUAD_PATTERNED_BUMP:
        uniformName = "quadPatternedBump";
        break;

    case TextureRole::NO_TEX:
        return;
    }

    this->setTexture(uniformName, t);
}

}}
