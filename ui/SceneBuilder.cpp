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

    auto &c = scene.getCamera();
    glm::vec3 d = lightDirection.x * c.getU().xyz()
                + lightDirection.y * c.getV().xyz()
                + lightDirection.z * c.getW().xyz()
                ;

    SceneLightData lightData;
    lightData.type = LightType::LIGHT_DIRECTIONAL,
    lightData.dir = glm::vec4(d, 0),
    lightData.color = {1,1,1,1},
    lightData.id = 0;

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
    // TODO: set gravity based on camera positon
//    auto &c = scene.getCamera();
//    glm::vec3 g = settings.gravity.x * c.getU().xyz() - glm::vec3(2.0f);
//                + settings.gravity.y * c.getV().xyz() - glm::vec3(2.0f);
//                + settings.gravity.z * c.getW().xyz() - glm::vec3(2.0f);
//                ;

    auto &physicsScene = scene.getPhysicsScene();
    physicsScene.setGlobal(PhysicsGlobalData {
        .damping = 0.0,
        .gravity = settings.gravity * STD_G / 10.f / 1000.f
    });

    setSphereDataFromSettings();
    setBoxDataFromSettings();
}

void SceneBuilder::createBox(Scene &scene) {
    auto &physicsScene = scene.getPhysicsScene();

    // inward-faced cube
    // -z
    auto extTransform = glm::translate(glm::vec3(0, 0, -1)) * glm::scale(glm::vec3(2.0f));

    auto planeObject_ptr = scene.createObject(m_quadObjectData);

    auto planePhysics_ptr = physicsScene.createObject(m_quadPhysicsData);
    planePhysics_ptr->setModelTransform(extTransform);

    planeObject_ptr->assignPhysics(planePhysics_ptr);

    // +z
    extTransform = glm::translate(glm::vec3(0, 0, 1));
    extTransform *= glm::rotate(PI, glm::vec3(1, 0, 0));
    extTransform *= glm::scale(glm::vec3(2.0f));

    planeObject_ptr = scene.createObject(m_quadObjectData);

    planePhysics_ptr = physicsScene.createObject(m_quadPhysicsData);
    planePhysics_ptr->setModelTransform(extTransform);

    planeObject_ptr->assignPhysics(planePhysics_ptr);


    // -y
    extTransform = glm::translate(glm::vec3(0, -1, 0));
    extTransform *= glm::rotate(-PI/2, glm::vec3(1, 0, 0));
    extTransform *= glm::scale(glm::vec3(2.0f));

    planeObject_ptr = scene.createObject(m_quadObjectData);

    planePhysics_ptr = physicsScene.createObject(m_quadPhysicsData);
    planePhysics_ptr->setModelTransform(extTransform);

    planeObject_ptr->assignPhysics(planePhysics_ptr);

    // +y
    extTransform = glm::translate(glm::vec3(0, 1, 0));
    extTransform *= glm::rotate(PI/2, glm::vec3(1, 0, 0));
    extTransform *= glm::scale(glm::vec3(2.0f));

    planeObject_ptr = scene.createObject(m_quadObjectData);

    planePhysics_ptr = physicsScene.createObject(m_quadPhysicsData);
    planePhysics_ptr->setModelTransform(extTransform);

    planeObject_ptr->assignPhysics(planePhysics_ptr);


    // -x
    extTransform = glm::translate(glm::vec3(-1, 0, 0));
    extTransform *= glm::rotate(PI/2, glm::vec3(0, 1, 0));
    extTransform *= glm::scale(glm::vec3(2.0f));

    planeObject_ptr = scene.createObject(m_quadObjectData);

    planePhysics_ptr = physicsScene.createObject(m_quadPhysicsData);
    planePhysics_ptr->setModelTransform(extTransform);

    planeObject_ptr->assignPhysics(planePhysics_ptr);

    // +x
    extTransform = glm::translate(glm::vec3(1, 0, 0));
    extTransform *= glm::rotate(-PI/2, glm::vec3(0, 1, 0));
    extTransform *= glm::scale(glm::vec3(2.0f));

    planeObject_ptr = scene.createObject(m_quadObjectData);

    planePhysics_ptr = physicsScene.createObject(m_quadPhysicsData);
    planePhysics_ptr->setModelTransform(extTransform);

    planeObject_ptr->assignPhysics(planePhysics_ptr);
}

void SceneBuilder::createBall(Scene &scene) {
    auto &physicsScene = scene.getPhysicsScene();

    // init scene objects
    auto sphereObject_ptr = scene.createObject(m_sphereObjectData);

    auto spherePhysics_ptr = physicsScene.createObject(m_spherePhysicsData);

    auto extTransform = glm::scale(glm::vec3(settings.radius/0.5f));

    spherePhysics_ptr->setLinearVelocity(settings.velocity / 1000.f);
    spherePhysics_ptr->setModelTransform(extTransform);

    sphereObject_ptr->pushMatrix(extTransform);
    sphereObject_ptr->assignPhysics(spherePhysics_ptr);
}
