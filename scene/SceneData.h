#ifndef SCENE_DATA_H
#define SCENE_DATA_H

#include <vector>
#include <string>

#include "lib/CommonData.h"

enum class LightType {
    LIGHT_POINT, LIGHT_DIRECTIONAL, LIGHT_SPOT, LIGHT_AREA
};

enum class PrimitiveType {
    PRIMITIVE_NONE,
    PRIMITIVE_CUBE,
    PRIMITIVE_CONE,
    PRIMITIVE_CYLINDER,
    PRIMITIVE_TORUS,
    PRIMITIVE_SPHERE,
    PRIMITIVE_QUAD,
    PRIMITIVE_MESH
};

// Scene global color coefficients
struct SceneGlobalData  {
    float ka = 1.0f;  // global ambient coefficient
    float kd = 1.0f;  // global diffuse coefficient
    float ks = 1.0f;  // global specular coefficient
    float kt = 1.0f;  // global transparency coefficient
};

// Data for a single light
struct SceneLightData {
    int id = -1;
    LightType type = LightType::LIGHT_POINT;

    RGBAf color {0.0f};
    glm::vec3 function;  // Attenuation function

    glm::vec4 pos;       // Not applicable to directional lights
    glm::vec4 dir;       // Not applicable to point lights

    float radius;        // Only applicable to spot lights
    float penumbra;      // Only applicable to spot lights
    float angle;         // Only applicable to spot lights

    float width, height; // Only applicable to area lights
};

// Data for scene camera
struct SceneCameraData {
    glm::vec4 pos  {0.0f};
    glm::vec4 look {0.0f};
    glm::vec4 up   {0.0f};

    float heightAngle = 0.0f;
    float aspectRatio = 1.0f;

    float aperture    = 0.0f;   // Only applicable for depth of field
    float focalLength = 2.0f;   // Only applicable for depth of field
};

// Data for file maps (ie: texture maps)
struct SceneFileMap {
    bool isUsed = false;
    std::string filename;
    float repeatU = 0.0f;
    float repeatV = 0.0f;
};

// Data for scene materials
struct SceneMaterial {
    // This field specifies the diffuse color of the object. This is the color you need to use for
    // the object in sceneview. You can get away with ignoring the other color values until
    // intersect and ray.
    RGBAf cDiffuse      {0.0f};
    RGBAf cAmbient      {0.0f};
    RGBAf cReflective   {0.0f};
    RGBAf cSpecular     {0.0f};
    RGBAf cTransparent  {0.0f};
    RGBAf cEmissive     {0.0f};

    SceneFileMap textureMap;
    float blend = 0.0f;

    SceneFileMap bumpMap;

    float shininess = 1.0f;

    float ior = 1.0f; // index of refraction
};

struct ScenePrimitiveData {
    PrimitiveType type = PrimitiveType::PRIMITIVE_NONE;
    std::string meshfile; // Only applicable to meshes
    SceneMaterial material;
};

// Structure for non-primitive scene objects
struct SceneObjectData {
    std::vector<CS123Transformation> transformations;

    std::vector<ScenePrimitiveData> primitives;

    std::vector<SceneObjectData> children;
};

#endif

