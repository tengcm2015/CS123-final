#ifndef PROJECT_SCENE_H
#define PROJECT_SCENE_H

#include <memory>

#include "gl/shaders/CS123Shader.h"
#include "lib/SceneData.h"
#include "SceneObject.h"
#include "ScenePrimitive.h"
#include "SceneCamera.h"

class View;

class Scene {
public:
    Scene();

    virtual ~Scene();

    SceneCamera &getCamera() { return m_camera; }
    void render(View *context, int msecLapsed = 0);

private:
    SceneCamera m_camera;
    std::unique_ptr<CS123::GL::CS123Shader> m_phongShader;

    SceneGlobalData m_globalData;
    std::vector<SceneLightData> m_sceneLights;
    std::shared_ptr<SceneObject> m_root_obj_ptr;
    std::vector<std::shared_ptr<SceneObject>> m_object_ptrs;
    std::vector<std::shared_ptr<ScenePrimitive>> m_primitive_ptrs;

    std::shared_ptr<SceneObject> addObject(const SceneObjectData &data);

    std::shared_ptr<ScenePrimitive> addPrimitive(const ScenePrimitiveData &data);

    void addLight(const SceneLightData &sceneLight);

    void setGlobal(const SceneGlobalData &global);

    void loadPhongShader();

    void initializeSceneLight();

    void initializeCamera();

    void populateScene();

    void setPhongSceneUniforms();

    void setMatrixUniforms(CS123::GL::Shader *shader, View *context);

    void clearLights();

    void setLights();

    void renderGeometry(CS123::GL::Shader *shader);
};


#endif //PROJECT_SCENE_H
