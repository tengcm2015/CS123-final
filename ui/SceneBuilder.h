#ifndef SCENEBUILDER_H
#define SCENEBUILDER_H

#include "scene/Scene.h"
#include "physics/PhysicsScene.h"

class SceneBuilder {
public:
    SceneBuilder();
    ~SceneBuilder();

    void initScene(Scene &scene);
    void createBall(Scene &scene);
    void setParametersFromSettings(Scene &scene);

private:
    void createBox(Scene &scene);

    void setSphereDataFromSettings();
    void setBoxDataFromSettings();

    SceneObjectData m_sphereObjectData;
    SceneObjectData m_quadObjectData;

    SceneMaterial m_sphereMaterial;
    SceneMaterial m_quadMaterial;

    CS123::PHYSICS::PhysicsObjectData m_spherePhysicsData;
    CS123::PHYSICS::PhysicsObjectData m_quadPhysicsData;

    CS123::PHYSICS::PhysicsMaterial m_spherePhysicsMaterial;
    CS123::PHYSICS::PhysicsMaterial m_quadPhysicsMaterial;
};


#endif //SCENEBUILDER_H
