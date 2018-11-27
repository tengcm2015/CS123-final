#include "Scene.h"
#include "ui/view.h"
#include "lib/ResourceLoader.h"
#include "gl/textures/Texture2D.h"

#include <sstream>

#define MAX_NUM_LIGHTS 10

using namespace CS123::GL;
using namespace CS123::PHYSICS;

Scene::Scene()
: m_camera()
{
    setGlobal({1,1,1,1});
    initializeCamera();
    initializeSceneLight();
    initializePhysics();
    loadPhongShader();
    populateScene();
}

Scene::~Scene()
{
}

void Scene::populateScene() {
    // init scene object
    SceneObjectData sphereObjectData;
    ScenePrimitiveData spherePrimitivedata {
        .type = PrimitiveType::PRIMITIVE_SPHERE,
        .material = {
            .cAmbient  = {0.2f, 0.1f, 0.0f, 1.0f},
            .cDiffuse  = {1.0f, 0.5f, 0.0f, 1.0f},
            .cSpecular = {1.0f, 1.0f, 1.0f, 1.0f},
            .shininess = 64
        }
    };

    sphereObjectData.primitives.push_back(spherePrimitivedata);
    auto sphereObject_ptr = this->createObject(sphereObjectData);

    auto spherePhysics = m_physicsScene.createObject(PhysicsObjectData{
            .type = GeometryType::GEOMETRY_SPHERE,
            .flag = PhysicsFlag::FLAG_DYNAMIC,
            .material = PhysicsMaterial{
                    .density = 1.0f,
                    .restitution = 1.0f,
                    .staticFriction = 0.5f,
                    .dynamicFriction = 0.4f
            }
    });

    sphereObject_ptr->assignPhysics(spherePhysics);
}

void Scene::loadPhongShader() {
    std::string vertexSource = ResourceLoader::loadResourceFileToString(":/shaders/phong.vert");
    std::string fragmentSource = ResourceLoader::loadResourceFileToString(":/shaders/phong.frag");
    m_phongShader = std::make_unique<CS123Shader>(vertexSource, fragmentSource);
}

void Scene::initializeSceneLight() {
    // Use a white directional light from the upper left corner
    auto lightDirection = glm::normalize(glm::vec4(1.f, -1.f, -1.f, 0.f));
    this->addLight(SceneLightData {
        .type = LightType::LIGHT_DIRECTIONAL,
        .dir = lightDirection,
        .color = {1,1,1,1},
        .id = 0
    });
}

void Scene::initializePhysics() {
    m_physicsScene.setGlobal(PhysicsGlobalData {
        .MPU = 10.0f,
        .damping = 0.0,
        .gravity = {0, -STD_G, 0}
    });
}

void Scene::initializeCamera() {
    /* use default for now */
}

void Scene::setPhongSceneUniforms() {
    m_phongShader->setUniform("useLighting", true);
    m_phongShader->setUniform("useArrowOffsets", false);
}

void Scene::setMatrixUniforms(Shader *shader, View *context) {
    shader->setUniform("p", m_camera.getProjectionMatrix());
    shader->setUniform("v", m_camera.getViewMatrix());
}

void Scene::clearLights() {
    for (int i = 0; i < MAX_NUM_LIGHTS; ++i) {
        std::ostringstream os;
        os << i;
        std::string indexString = "[" + os.str() + "]"; // e.g. [0], [1], etc.
        m_phongShader->setUniform("lightColors" + indexString, glm::vec3(0.0f, 0.0f, 0.0f));
    }
}

void Scene::setLights() {
    clearLights();
    for (auto &l: m_sceneLights)
        m_phongShader->setLight(l);
}

void Scene::render(View *context, int msecLapsed) {
    // update physics
    m_physicsScene.simulate(msecLapsed);
    for (auto &op: m_object_ptrs)
        op->update();

    // render scene
    glClearColor(0, 0, 0, 0);

    m_phongShader->bind();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    setLights();
    setPhongSceneUniforms();
    setMatrixUniforms(m_phongShader.get(), context);
    renderGeometry(m_phongShader.get());

    m_phongShader->unbind();
}

void Scene::renderGeometry(Shader* shader) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    for (auto &pp: m_primitive_ptrs) {
        shader->setUniform("m", pp->getModelTransform());

        if (shader == m_phongShader.get()) {
            auto material = pp->getMaterial();
            material.cAmbient     *= m_globalData.ka;
            material.cDiffuse     *= m_globalData.kd;
            material.cSpecular    *= m_globalData.ks;
            material.cTransparent *= m_globalData.kt;
            m_phongShader->applyMaterial(material);

            if (pp->textureUsed()) {
                m_phongShader->setUniform("useTexture", 1);
                m_phongShader->setUniform("repeatUV", glm::vec2(material.textureMap.repeatU, material.textureMap.repeatV));
                m_phongShader->setTexture("tex", pp->getTexture2D());
            }

            pp->draw();

            if (pp->textureUsed()) {
                m_phongShader->setUniform("useTexture", 0);
                pp->getTexture2D().unbind();
            }

        } else
            pp->draw();
    }
}

std::shared_ptr<SceneObject> Scene::createObject(const SceneObjectData &data)
{
    auto obj_ptr = std::make_shared<SceneObject>(data);
    m_object_ptrs.push_back(obj_ptr);

    SceneObject &sceneObject = *obj_ptr;

    for (auto &p: data.primitives)
        sceneObject.assignChild(this->createPrimitive(p));

    for (auto &c: data.children)
        sceneObject.assignChild(this->createObject(c));

    return obj_ptr;
}

std::shared_ptr<ScenePrimitive> Scene::createPrimitive(const ScenePrimitiveData &scenePrimitive) {
    auto primitive_ptr = std::make_shared<ScenePrimitive>(scenePrimitive);
    m_primitive_ptrs.push_back(primitive_ptr);
    return primitive_ptr;
}

void Scene::addLight(const SceneLightData &sceneLight) {
    m_sceneLights.push_back(sceneLight);
}

void Scene::setGlobal(const SceneGlobalData &global) {
    m_globalData = global;
}


