#ifndef SCENE_DATA_H
#define SCENE_DATA_H

#include <vector>
#include <string>

#include "lib/CommonData.h"

enum class LightType {
    LIGHT_POINT, LIGHT_DIRECTIONAL, LIGHT_SPOT, LIGHT_AREA
};

enum class PrimitiveType {
    PRIMITIVE_CUBE,
    PRIMITIVE_CONE,
    PRIMITIVE_CYLINDER,
    PRIMITIVE_TORUS,
    PRIMITIVE_SPHERE,
    PRIMITIVE_MESH
};

// Struct to store a RGBA color in floats [0,1]
using SceneColor = glm::vec4;

// Scene global color coefficients
struct SceneGlobalData  {
   float ka;  // global ambient coefficient
   float kd;  // global diffuse coefficient
   float ks;  // global specular coefficient
   float kt;  // global transparency coefficient
};

// Data for a single light
struct SceneLightData {
   int id;
   LightType type;

   SceneColor color;
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
   glm::vec4 pos;
   glm::vec4 look;
   glm::vec4 up;

   float heightAngle;
   float aspectRatio;

   float aperture;      // Only applicable for depth of field
   float focalLength;   // Only applicable for depth of field
};

// Data for file maps (ie: texture maps)
struct SceneFileMap {
//    SceneFileMap() : texid(0) {}
   bool isUsed;
   std::string filename;
   float repeatU;
   float repeatV;

   void clear() {
       isUsed = false;
       repeatU = 0.0f;
       repeatV = 0.0f;
       filename = std::string();
   }
};

// Data for scene materials
struct SceneMaterial {
   // This field specifies the diffuse color of the object. This is the color you need to use for
   // the object in sceneview. You can get away with ignoring the other color values until
   // intersect and ray.
//   SceneMaterial() {}
   SceneColor cDiffuse;

   SceneColor cAmbient;
   SceneColor cReflective;
   SceneColor cSpecular;
   SceneColor cTransparent;
   SceneColor cEmissive;

   SceneFileMap textureMap;
   float blend;

   SceneFileMap bumpMap;

   float shininess;

   float ior; // index of refraction

   void clear() {
       cAmbient.r = 0.0f; cAmbient.g = 0.0f; cAmbient.b = 0.0f; cAmbient.a = 0.0f;
       cDiffuse.r = 0.0f; cDiffuse.g = 0.0f; cDiffuse.b = 0.0f; cDiffuse.a = 0.0f;
       cSpecular.r = 0.0f; cSpecular.g = 0.0f; cSpecular.b = 0.0f; cSpecular.a = 0.0f;
       cReflective.r = 0.0f; cReflective.g = 0.0f; cReflective.b = 0.0f; cReflective.a = 0.0f;
       cTransparent.r = 0.0f; cTransparent.g = 0.0f; cTransparent.b = 0.0f; cTransparent.a = 0.0f;
       cEmissive.r = 0.0f; cEmissive.g = 0.0f; cEmissive.b = 0.0f; cEmissive.a = 0.0f;
       textureMap.clear();
       bumpMap.clear();
       blend = 0.0f;
       shininess = 0.0f;
       ior = 0.0;
   }
};

struct ScenePrimitiveData {
   PrimitiveType type;
   std::string meshfile;     // Only applicable to meshes
   SceneMaterial material;
};

// Structure for non-primitive scene objects
struct SceneObjectData {
   std::vector<CS123::UTIL::Transformation> transformations;

   std::vector<ScenePrimitiveData> primitives;

   std::vector<SceneObjectData> children;
};

#endif

