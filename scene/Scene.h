#ifndef SCENE_H
#define SCENE_H

#include "SceneData.h"
#include "SceneObject.h"
#include "ScenePrimitive.h"
#include "SceneCamera.h"

#include "gl/FullScreenQuad.h"
#include "gl/datatype/FBO.h"
#include "gl/shaders/PhongShader.h"
#include "gl/shaders/RayMarchShader.h"
#include "physics/PhysicsScene.h"

#include <memory>

class View;

class Scene {
public:
    Scene();

    virtual ~Scene();

    SceneCamera &getCamera() { return m_camera; }

    CS123::PHYSICS::PhysicsScene &getPhysicsScene() { return m_physicsScene; }

    std::shared_ptr<SceneObject> createObject(const SceneObjectData &data);

    std::shared_ptr<ScenePrimitive> createPrimitive(const ScenePrimitiveData &data);

    void addLight(const SceneLightData &sceneLight);

    void setGlobal(const SceneGlobalData &global);

    void setGravity(glm::vec3 gravity);

    void setScreenSize(int w, int h);

    void clearLights();
    void clearObjects();

    void render(View *context, int msecLapsed = 0);

    void mouseDown(float deltaX, float deltaY);
    void mouseDragged(float deltaX, float deltaY);
    void mouseUp(float deltaX, float deltaY);
    void mouseScrolled(float delta);

private:
    int m_width, m_height;

    SceneCamera m_camera;
    float m_cam_angleX, m_cam_angleY, m_zoomZ;
    glm::vec3 m_gravity;
    glm::mat4 m_gravityCoord_mat;

    bool m_isDragging;
    int oldX, oldY;

    CS123::GL::FullScreenQuad m_fullScreenQuad;
    std::unique_ptr<CS123::GL::FBO> m_tmp_FBO;
    std::unique_ptr<CS123::GL::RayMarchShader> m_renderShader;
    std::unique_ptr<CS123::GL::PhongShader> m_phongShader;
    std::unique_ptr<CS123::GL::Shader> m_filterShader;

    SceneGlobalData m_globalData;
    std::vector<SceneLightData> m_sceneLights;
    std::shared_ptr<SceneObject> m_root_obj_ptr;
    std::vector<std::shared_ptr<SceneObject>> m_object_ptrs;
    std::vector<std::shared_ptr<ScenePrimitive>> m_primitive_ptrs;

    CS123::PHYSICS::PhysicsScene m_physicsScene;

    void setPhongSceneUniforms();
    void setRaymarchSceneUniforms();

    void __render();
    void renderGeometry(CS123::GL::Shader *shader);

    SceneLightData alignLightWithCamera(const SceneLightData &l);
    void updateGravity();
    void updateSceneCamera();
};


#endif //PROJECT_SCENE_H
