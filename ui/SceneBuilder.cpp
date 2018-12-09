#include "SceneBuilder.h"

#include "glm/gtx/transform.hpp"  // glm::translate, scale, rotate

#include "physics/PhysicsScene.h"

using namespace CS123::GL;
using namespace CS123::PHYSICS;

#define PI static_cast<float>(M_PI)

static void initializeSceneLight(Scene &scene) {
    // Use a white directional light from the upper left corner
    auto lightDirection = glm::normalize(glm::vec4(1.f, -1.f, -1.f, 0.f));\

    SceneLightData lightData;
    lightData.type = LightType::LIGHT_DIRECTIONAL,
    lightData.dir = lightDirection,
    lightData.color = {1,1,1,1},
    lightData.id = 0;

    scene.addLight(lightData);
}

void SceneBuilder::initScene(Scene &scene) {
    initializeSceneLight(scene);

    // init scene objects
    auto &physicsScene = scene.getPhysicsScene();
    physicsScene.setGlobal(PhysicsGlobalData {
        .MPU = 10.0f,
        .damping = 0.0,
        .gravity = {0, -STD_G, 0}
    });

    // sphere
    SceneObjectData sphereObjectData;

    ScenePrimitiveData spherePrimitiveData;
    spherePrimitiveData.type = PrimitiveType::PRIMITIVE_SPHERE;
    spherePrimitiveData.meshfile = "";

    SceneMaterial material;
    material.cDiffuse  = {1.0f, 0.5f, 0.0f, 1.0f};
    material.cAmbient  = {0.2f, 0.1f, 0.0f, 1.0f};
    material.cSpecular = {1.0f, 1.0f, 1.0f, 1.0f};
    material.shininess = 64;
    spherePrimitiveData.material = material;

    sphereObjectData.primitives.push_back(spherePrimitiveData);
    auto sphereObject_ptr = scene.createObject(sphereObjectData);

    PhysicsObjectData spherePhysicsData;
    spherePhysicsData.type = GeometryType::GEOMETRY_SPHERE;
    spherePhysicsData.flag = PhysicsFlag::FLAG_DYNAMIC;

    PhysicsMaterial physicsMaterial;
    physicsMaterial.density = 1.0f;
    physicsMaterial.restitution = 1.0f;
    physicsMaterial.staticFriction = 0.5f;
    physicsMaterial.dynamicFriction = 0.4f;
    spherePhysicsData.material = physicsMaterial;

    auto spherePhysics_ptr = physicsScene.createObject(spherePhysicsData);

    spherePhysics_ptr->setLinearVelocity(glm::vec3(0.001,0.002,-0.001));
    sphereObject_ptr->pushMatrix(glm::scale(glm::vec3(0.3f)));
    sphereObject_ptr->assignPhysics(spherePhysics_ptr);

    // sphere 2
    sphereObject_ptr = scene.createObject(sphereObjectData);
    spherePhysics_ptr = physicsScene.createObject(spherePhysicsData);

    spherePhysics_ptr->setLinearVelocity(glm::vec3(0.003,0.001,0.002));
    spherePhysics_ptr->setModelTransform(glm::translate(glm::vec3(0.5, 0.5, 0.5)));

    sphereObject_ptr->pushMatrix(glm::scale(glm::vec3(0.3f)));
    sphereObject_ptr->assignPhysics(spherePhysics_ptr);


    // inward-faced cube
    SceneObjectData &planeObjectData = sphereObjectData;

    ScenePrimitiveData &planePrimitiveData = spherePrimitiveData;
    planePrimitiveData.type = PrimitiveType::PRIMITIVE_QUAD;
    planePrimitiveData.meshfile = "";

    material.cDiffuse  = {0.5f, 0.5f, 0.5f, 1.0f};
    material.cAmbient  = {0.1f, 0.1f, 0.1f, 1.0f};
    material.cSpecular = {1.0f, 1.0f, 1.0f, 1.0f};
    material.shininess = 64;
    planePrimitiveData.material = material;

    planeObjectData.primitives[0] = planePrimitiveData;

    PhysicsObjectData &planePhysicsData = spherePhysicsData;
    planePhysicsData.type = GeometryType::GEOMETRY_PLANE;
    planePhysicsData.flag = PhysicsFlag::FLAG_KINEMATIC;

    // -z
    auto extTransform = glm::translate(glm::vec3(0, 0, -1)) * glm::scale(glm::vec3(2.0f));

    auto planeObject_ptr = scene.createObject(planeObjectData);

    auto planePhysics_ptr = physicsScene.createObject(planePhysicsData);
    planePhysics_ptr->setModelTransform(extTransform);

    planeObject_ptr->assignPhysics(planePhysics_ptr);

    // +z
    extTransform = glm::translate(glm::vec3(0, 0, 1));
    extTransform *= glm::rotate(PI, glm::vec3(1, 0, 0));
    extTransform *= glm::scale(glm::vec3(2.0f));

    planeObject_ptr = scene.createObject(planeObjectData);

    planePhysics_ptr = physicsScene.createObject(planePhysicsData);
    planePhysics_ptr->setModelTransform(extTransform);

    planeObject_ptr->assignPhysics(planePhysics_ptr);


    // -y
    extTransform = glm::translate(glm::vec3(0, -1, 0));
    extTransform *= glm::rotate(-PI/2, glm::vec3(1, 0, 0));
    extTransform *= glm::scale(glm::vec3(2.0f));

    planeObject_ptr = scene.createObject(planeObjectData);

    planePhysics_ptr = physicsScene.createObject(planePhysicsData);
    planePhysics_ptr->setModelTransform(extTransform);

    planeObject_ptr->assignPhysics(planePhysics_ptr);

    // +y
    extTransform = glm::translate(glm::vec3(0, 1, 0));
    extTransform *= glm::rotate(PI/2, glm::vec3(1, 0, 0));
    extTransform *= glm::scale(glm::vec3(2.0f));

    planeObject_ptr = scene.createObject(planeObjectData);

    planePhysics_ptr = physicsScene.createObject(planePhysicsData);
    planePhysics_ptr->setModelTransform(extTransform);

    planeObject_ptr->assignPhysics(planePhysics_ptr);


    // -x
    extTransform = glm::translate(glm::vec3(-1, 0, 0));
    extTransform *= glm::rotate(PI/2, glm::vec3(0, 1, 0));
    extTransform *= glm::scale(glm::vec3(2.0f));

    planeObject_ptr = scene.createObject(planeObjectData);

    planePhysics_ptr = physicsScene.createObject(planePhysicsData);
    planePhysics_ptr->setModelTransform(extTransform);

    planeObject_ptr->assignPhysics(planePhysics_ptr);

    // +x
    extTransform = glm::translate(glm::vec3(1, 0, 0));
    extTransform *= glm::rotate(-PI/2, glm::vec3(0, 1, 0));
    extTransform *= glm::scale(glm::vec3(2.0f));

    planeObject_ptr = scene.createObject(planeObjectData);

    planePhysics_ptr = physicsScene.createObject(planePhysicsData);
    planePhysics_ptr->setModelTransform(extTransform);

    planeObject_ptr->assignPhysics(planePhysics_ptr);

}
