#include "SceneBuilder.h"

#include "Settings.h"
#include "lib/CS123Utils.h"
#include "glm/gtx/transform.hpp"  // glm::translate, scale, rotate

using namespace CS123::GL;
using namespace CS123::PHYSICS;
using namespace CS123::UTILS;

#define PI static_cast<float>(M_PI)

static void initializeSceneLight(Scene &scene) {
    // Use a white directional light from the upper left corner
    auto lightDirection = glm::normalize(glm::vec4(1.f, -1.f, -1.f, 0.f));

    SceneLightData lightData;
    lightData.type = LightType::LIGHT_DIRECTIONAL,
    lightData.dir = lightDirection,
    lightData.color = {1,1,1,1},

    scene.addLight(lightData);
}

SceneBuilder::SceneBuilder()
{
    setSphereDataFromSettings();
    setBoxDataFromSettings();
}

SceneBuilder::~SceneBuilder()
{
}

void SceneBuilder::initScene(Scene &scene) {
    initializeSceneLight(scene);
    createBox(scene);
}

void SceneBuilder::setSphereDataFromSettings() {
    ScenePrimitiveData spherePrimitiveData;
    spherePrimitiveData.type = PrimitiveType::PRIMITIVE_SPHERE;
    spherePrimitiveData.meshfile = "";

    RGBAf color = byte2REAL(settings.ballColor);
    m_sphereMaterial.cDiffuse  = color;
    m_sphereMaterial.cAmbient  = color / 5.f;
    m_sphereMaterial.cSpecular = RGBAf(1.0f);
    m_sphereMaterial.shininess = settings.shininess;

    switch (settings.textureType) {
        case TextureType::TEXTURE_SMOOTH:
            m_sphereMaterial.textureMap.isUsed = true;
            m_sphereMaterial.textureMap.filename = ":/textures/sphere_smooth.jpg";
            m_sphereMaterial.textureMap.repeatU = 1;
            m_sphereMaterial.textureMap.repeatV = 1;
            m_sphereMaterial.blend = 1;
            m_sphereMaterial.textureMap.role = TextureRole::SPHERE_SMOOTH_TEX;
            break;

        case TextureType::TEXTURE_PATTERNED:
            m_sphereMaterial.textureMap.isUsed = true;
            m_sphereMaterial.textureMap.filename = ":/textures/sphere_patterned.jpg";
            m_sphereMaterial.textureMap.repeatU = 1;
            m_sphereMaterial.textureMap.repeatV = 1;
            m_sphereMaterial.blend = 1;
            m_sphereMaterial.textureMap.role = TextureRole::SPHERE_PATTERNED_TEX;

            m_sphereMaterial.bumpMap.isUsed = true;
            m_sphereMaterial.bumpMap.filename = ":/textures/sphere_bump.jpg";
            m_sphereMaterial.bumpMap.repeatU = 1;
            m_sphereMaterial.bumpMap.repeatV = 1;
            m_sphereMaterial.bumpMap.role = TextureRole::SPHERE_PATTERNED_BUMP;

            break;

        case TextureType::TEXTURE_NONE:
        default:
            m_sphereMaterial.textureMap.isUsed = false;
            m_sphereMaterial.bumpMap.isUsed = false;
            break;
    }

    spherePrimitiveData.material = m_sphereMaterial;

    m_sphereObjectData.primitives.clear();
    m_sphereObjectData.primitives.push_back(spherePrimitiveData);

    m_spherePhysicsMaterial.density         = settings.density;
    m_spherePhysicsMaterial.restitution     = settings.restitution;
    m_spherePhysicsMaterial.staticFriction  = settings.friction;
    m_spherePhysicsMaterial.dynamicFriction = settings.friction * 0.8f;

    m_spherePhysicsData.type = GeometryType::GEOMETRY_SPHERE;
    m_spherePhysicsData.flag = PhysicsFlag::FLAG_DYNAMIC;

    m_spherePhysicsData.material = m_spherePhysicsMaterial;
}

void SceneBuilder::setBoxDataFromSettings() {
    ScenePrimitiveData quadPrimitiveData;
    quadPrimitiveData.type = PrimitiveType::PRIMITIVE_QUAD;
    quadPrimitiveData.meshfile = "";

    m_quadMaterial.cDiffuse  = {0.5f, 0.5f, 0.5f, 1.0f};
    m_quadMaterial.cAmbient  = {0.1f, 0.1f, 0.1f, 1.0f};
    m_quadMaterial.cSpecular = {1.0f, 1.0f, 1.0f, 1.0f};
    m_quadMaterial.shininess = 64;

    switch (settings.boxTextureType) {
        case TextureType::TEXTURE_SMOOTH:
            m_quadMaterial.textureMap.isUsed = true;
            m_quadMaterial.textureMap.filename = ":/textures/box_smooth.jpg";
            m_quadMaterial.textureMap.repeatU = 1;
            m_quadMaterial.textureMap.repeatV = 1;
            m_quadMaterial.blend = 1;
            m_quadMaterial.textureMap.role = TextureRole::QUAD_SMOOTH_TEX;
            break;

        case TextureType::TEXTURE_PATTERNED:
            m_quadMaterial.textureMap.isUsed = true;
            m_quadMaterial.textureMap.filename = ":/textures/box_patterned.jpg";
            m_quadMaterial.textureMap.repeatU = 2;
            m_quadMaterial.textureMap.repeatV = 2;
            m_quadMaterial.blend = 1;
            m_quadMaterial.textureMap.role = TextureRole::QUAD_PATTERNED_TEX;

            // TODO: quad bump map

            break;

        case TextureType::TEXTURE_NONE:
        default:
            m_quadMaterial.textureMap.isUsed = false;
            m_quadMaterial.bumpMap.isUsed = false;
            break;
    }

    quadPrimitiveData.material = m_quadMaterial;


    m_quadObjectData.primitives.clear();
    m_quadObjectData.primitives.push_back(quadPrimitiveData);

    m_quadPhysicsMaterial.density = 1.0f;
    m_quadPhysicsMaterial.restitution = 1.0f;
    m_quadPhysicsMaterial.staticFriction = 0.5f;
    m_quadPhysicsMaterial.dynamicFriction = 0.4f;

    m_quadPhysicsData.type = GeometryType::GEOMETRY_PLANE;
    m_quadPhysicsData.flag = PhysicsFlag::FLAG_KINEMATIC;

    m_quadPhysicsData.material = m_quadPhysicsMaterial;
}

// standard gravity in m/(s^2)
const float STD_G = 9.80665;

void SceneBuilder::setParametersFromSettings(Scene &scene) {
    scene.setGravity(settings.gravity * STD_G  / 1000.f / 1000.f);
    setSphereDataFromSettings();
    setBoxDataFromSettings();
}

void SceneBuilder::applyBoxSettings() {
    for (auto &pp: m_box_primitive_ptrs) {
        if (auto sp = pp.lock())
            sp->setTexture(m_quadMaterial.textureMap);
    }
}

void SceneBuilder::createBox(Scene &scene) {
    m_box_primitive_ptrs.clear();

    auto &physicsScene = scene.getPhysicsScene();

    // inward-faced cube
    // -z
    auto extTransform = glm::translate(glm::vec3(0, 0, -1)) * glm::scale(glm::vec3(2.0f));

    auto planeObject_ptr = scene.createObject(m_quadObjectData);

    auto planePhysics_ptr = physicsScene.createObject(m_quadPhysicsData);
    planePhysics_ptr->setModelTransform(extTransform);

    planeObject_ptr->assignPhysics(planePhysics_ptr);

    m_box_primitive_ptrs.emplace_back(planeObject_ptr->getPrimitive(0));

    // +z
    extTransform = glm::translate(glm::vec3(0, 0, 1));
    extTransform *= glm::rotate(PI, glm::vec3(1, 0, 0));
    extTransform *= glm::scale(glm::vec3(2.0f));

    planeObject_ptr = scene.createObject(m_quadObjectData);

    planePhysics_ptr = physicsScene.createObject(m_quadPhysicsData);
    planePhysics_ptr->setModelTransform(extTransform);

    planeObject_ptr->assignPhysics(planePhysics_ptr);

    m_box_primitive_ptrs.emplace_back(planeObject_ptr->getPrimitive(0));


    // -y
    extTransform = glm::translate(glm::vec3(0, -1, 0));
    extTransform *= glm::rotate(-PI/2, glm::vec3(1, 0, 0));
    extTransform *= glm::scale(glm::vec3(2.0f));

    planeObject_ptr = scene.createObject(m_quadObjectData);

    planePhysics_ptr = physicsScene.createObject(m_quadPhysicsData);
    planePhysics_ptr->setModelTransform(extTransform);

    planeObject_ptr->assignPhysics(planePhysics_ptr);

    m_box_primitive_ptrs.emplace_back(planeObject_ptr->getPrimitive(0));

    // +y
    extTransform = glm::translate(glm::vec3(0, 1, 0));
    extTransform *= glm::rotate(PI/2, glm::vec3(1, 0, 0));
    extTransform *= glm::scale(glm::vec3(2.0f));

    planeObject_ptr = scene.createObject(m_quadObjectData);

    planePhysics_ptr = physicsScene.createObject(m_quadPhysicsData);
    planePhysics_ptr->setModelTransform(extTransform);

    planeObject_ptr->assignPhysics(planePhysics_ptr);

    m_box_primitive_ptrs.emplace_back(planeObject_ptr->getPrimitive(0));


    // -x
    extTransform = glm::translate(glm::vec3(-1, 0, 0));
    extTransform *= glm::rotate(PI/2, glm::vec3(0, 1, 0));
    extTransform *= glm::scale(glm::vec3(2.0f));

    planeObject_ptr = scene.createObject(m_quadObjectData);

    planePhysics_ptr = physicsScene.createObject(m_quadPhysicsData);
    planePhysics_ptr->setModelTransform(extTransform);

    planeObject_ptr->assignPhysics(planePhysics_ptr);

    m_box_primitive_ptrs.emplace_back(planeObject_ptr->getPrimitive(0));

    // +x
    extTransform = glm::translate(glm::vec3(1, 0, 0));
    extTransform *= glm::rotate(-PI/2, glm::vec3(0, 1, 0));
    extTransform *= glm::scale(glm::vec3(2.0f));

    planeObject_ptr = scene.createObject(m_quadObjectData);

    planePhysics_ptr = physicsScene.createObject(m_quadPhysicsData);
    planePhysics_ptr->setModelTransform(extTransform);

    planeObject_ptr->assignPhysics(planePhysics_ptr);

    m_box_primitive_ptrs.emplace_back(planeObject_ptr->getPrimitive(0));
}

void SceneBuilder::createBall(Scene &scene) {
    auto &physicsScene = scene.getPhysicsScene();

    // init scene objects
    auto sphereObject_ptr = scene.createObject(m_sphereObjectData);

    auto spherePhysics_ptr = physicsScene.createObject(m_spherePhysicsData);

    auto extTransform = glm::scale(glm::vec3(settings.radius/0.5f));

    spherePhysics_ptr->setLinearVelocity(settings.velocity / 1000.f);
    spherePhysics_ptr->setModelTransform(extTransform);

    //sphereObject_ptr->pushMatrix(extTransform);
    sphereObject_ptr->assignPhysics(spherePhysics_ptr);
}
