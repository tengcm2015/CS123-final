#include "RayMarchShader.h"

#include "scene/SceneData.h"
#include "gl/GLDebug.h"

#include <sstream>

namespace CS123 { namespace GL {

RayMarchShader::RayMarchShader(const std::string &vertexSource, const std::string &fragmentSource)
: Shader(vertexSource, fragmentSource)
{
    m_texture2D_ptrs.fill(nullptr);
    for (int i = 0; i < RayMarchShader::MAX_PRIMITIVES; ++i)
        clearPrimitive(i);
}

RayMarchShader::RayMarchShader(const std::string &vertexSource, const std::string &geometrySource, const std::string &fragmentSource)
: Shader(vertexSource, geometrySource, fragmentSource)
{
    m_texture2D_ptrs.fill(nullptr);
    for (int i = 0; i < RayMarchShader::MAX_PRIMITIVES; ++i)
        clearPrimitive(i);
}


void RayMarchShader::loadPrimitive(int id) {
    auto pp = m_primitive_ptrs[id];
    if (pp == nullptr) {
        clearPrimitive(id);
        return;
    }

    setUniformArrayByIndex("primitiveType"     , UTIL::as_integer(pp->type()), id);
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
        setUniformArrayByIndex("primitiveTexID", id, id);
        setUniformArrayByIndex("primitiveBlend", m.blend, id);
        std::ostringstream os;
        os << id;
        std::string indexString = "[" + os.str() + "]"; // e.g. [0], [1], etc.
        setTexture(indexString, pp->getTexture2D());

    } else {
        setUniformArrayByIndex("primitiveTexID", -1, id);
    }

    if (m.bumpMap.isUsed) {
        setUniformArrayByIndex("primitiveBumpID", id, id);

    } else {
        setUniformArrayByIndex("primitiveBumpID", -1, id);
    }
}


void RayMarchShader::clearPrimitive(int id) {
    m_primitive_ptrs[id] = nullptr;
    setUniformArrayByIndex("primitiveType", UTIL::as_integer(PrimitiveType::PRIMITIVE_NONE), id);
    setUniformArrayByIndex("primitiveTexID"  , -1, id);
    setUniformArrayByIndex("primitiveBumpID" , -1, id);
}


void RayMarchShader::insertPrimitive(const std::shared_ptr<ScenePrimitive> &pp) {
    int id = -1;
    while (id < RayMarchShader::MAX_PRIMITIVES) {
        if (m_primitive_ptrs[++id] == nullptr) {
            m_primitive_ptrs[id] = pp;
            break;
        }
    }
    if (id >= RayMarchShader::MAX_PRIMITIVES)
        return;

    loadPrimitive(id);
}

void RayMarchShader::deletePrimitive(const std::shared_ptr<ScenePrimitive> &pp) {
    for (int i = 0; i < RayMarchShader::MAX_PRIMITIVES; ++i) {
        if (m_primitive_ptrs[i] == pp) {
            clearPrimitive(i);
            break;
        }
    }
}

void RayMarchShader::reloadPrimitives() {
    for (int i = 0; i < RayMarchShader::MAX_PRIMITIVES; ++i)
        loadPrimitive(i);
}

void RayMarchShader::setLight(const SceneLightData &light) {
    bool ignoreLight = false;

    GLint lightType;
    std::string name;
    glm::vec3 ndir;
    switch(light.type) {
        case LightType::LIGHT_POINT:
            lightType = 0;
            name = "lightPositions";
            setUniformArrayByIndex(name, light.pos.xyz(), light.id);
            //if (!settings.usePointLights) ignoreLight = true;
            break;
        case LightType::LIGHT_DIRECTIONAL:
            lightType = 1;
            ndir = glm::normalize(light.dir.xyz());
            name = "lightDirections";
            setUniformArrayByIndex(name, ndir, light.id);
            //if (!settings.useDirectionalLights) ignoreLight = true;
            break;
        default:
            lightType = 0;
            ignoreLight = true; // Light type not supported
            break;
    }

    SceneColor color = light.color;
    if (ignoreLight) color.r = color.g = color.b = 0;

    setUniformArrayByIndex("lightTypes", lightType, light.id);
    setUniformArrayByIndex("lightColors", glm::vec3(color.r, color.g, color.b), light.id);
//    setUniformArrayByIndex("lightAttenuations", light.function, light.id);
}

void RayMarchShader::clearLight(int id) {
    setUniformArrayByIndex("lightColors", glm::vec3(0.0f), id);
}


}}
