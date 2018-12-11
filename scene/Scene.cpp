#include "Scene.h"
#include "ui/view.h"
#include "ui/Settings.h"
#include "lib/ResourceLoader.h"
#include "gl/textures/Texture2D.h"

#include <sstream>

using namespace CS123::GL;
using namespace CS123::PHYSICS;

Scene::Scene()
: m_width(0), m_height(0)
, m_camera()
, m_fullScreenQuad()
, m_tmp_FBO(nullptr)
, m_renderShader(nullptr)
, m_phongShader(nullptr)
, m_filterShader(nullptr)
, m_globalData{1,1,1,1}
, m_physicsScene()
{

    // load ray march shader
    std::string vertexSource = ResourceLoader::loadResourceFileToString(":/shaders/quad.vert");
    std::string fragmentSource = ResourceLoader::loadResourceFileToString(":/shaders/rayMarcher.frag");
    m_renderShader = std::make_unique<RayMarchShader>(vertexSource, fragmentSource);

    // load phong shader
    vertexSource = ResourceLoader::loadResourceFileToString(":/shaders/phong.vert");
    fragmentSource = ResourceLoader::loadResourceFileToString(":/shaders/phong.frag");
    m_phongShader = std::make_unique<PhongShader>(vertexSource, fragmentSource);

    // load filter shader
    vertexSource = ResourceLoader::loadResourceFileToString(":/shaders/quad.vert");
    fragmentSource = ResourceLoader::loadResourceFileToString(":/shaders/FXAA.frag");
    m_filterShader = std::make_unique<Shader>(vertexSource, fragmentSource);
}

Scene::~Scene()
{
}

void Scene::setPhongSceneUniforms() {
    m_phongShader->setUniform("useLighting", true);
    m_phongShader->setUniform("useArrowOffsets", false);
}

void Scene::setMatrixUniforms(Shader *shader) {
    if (settings.useRaymarching) {
        shader->setUniform("eye" , m_camera.getPosition().xyz());
        shader->setUniform("look", m_camera.getLook().xyz());
        shader->setUniform("up"  , m_camera.getV().xyz());
        shader->setUniform("aspectRatio", m_camera.getAspectRatio());
    } else {
        shader->setUniform("p", m_camera.getProjectionMatrix());
        shader->setUniform("v", m_camera.getViewMatrix());
    }
}

void Scene::clearLights() {
    if (settings.useRaymarching) {
        for (int i = 0; i < RayMarchShader::MAX_NUM_LIGHTS; ++i)
            m_renderShader->clearLight(i);

    } else {
        for (int i = 0; i < PhongShader::MAX_NUM_LIGHTS; ++i)
            m_phongShader->clearLight(i);
    }
}

void Scene::setLights() {
    clearLights();
    if (settings.useRaymarching) {
        for (auto &l: m_sceneLights)
            m_renderShader->setLight(l);

    } else {
        for (auto &l: m_sceneLights)
            m_phongShader->setLight(l);
    }
}

void Scene::setScreenSize(int w, int h) {
    m_width = w, m_height = h;
    m_camera.setAspectRatio(static_cast<float>(w) / static_cast<float>(h));
    m_tmp_FBO = std::make_unique<FBO>(1, FBO::DEPTH_STENCIL_ATTACHMENT::DEPTH_ONLY, w, h,
                                       TextureParameters::WRAP_METHOD::CLAMP_TO_EDGE);
}

void Scene::__render() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (settings.useRaymarching) {
        m_renderShader->bind();

        setLights();
        m_renderShader->reloadPrimitives();
        setMatrixUniforms(m_renderShader.get());

        m_fullScreenQuad.draw();

        m_renderShader->unbind();

    } else {
        m_phongShader->bind();

        setLights();
        setPhongSceneUniforms();
        setMatrixUniforms(m_phongShader.get());

        renderGeometry(m_phongShader.get());

        m_phongShader->unbind();

    }
}

void Scene::render(View *context, int msecLapsed) {
    // update physics
    m_physicsScene.simulate(msecLapsed);
    for (auto &op: m_object_ptrs)
        op->update();

    // render scene
    glClearColor(0, 0, 0, 0);

    if (settings.useFXAA) {
        m_tmp_FBO->bind();

        __render();

        //m_tmp_FBO->unbind();
        context->makeCurrent();
        glViewport(0, 0, m_width, m_height);

        m_tmp_FBO->getColorAttachment(0).bind();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        m_filterShader->bind();
        m_fullScreenQuad.draw();
        m_filterShader->unbind();

        m_tmp_FBO->getColorAttachment(0).unbind();

    } else {
        __render();
    }
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

std::shared_ptr<SceneObject> Scene::createObject(const SceneObjectData &data) {
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
    if (settings.useRaymarching)
        m_renderShader->insertPrimitive(primitive_ptr);
    return primitive_ptr;
}

void Scene::addLight(const SceneLightData &sceneLight) {
    m_sceneLights.push_back(sceneLight);
}

void Scene::setGlobal(const SceneGlobalData &global) {
    m_globalData = global;
}

void Scene::clearObjects() {
    m_object_ptrs.clear();
    m_primitive_ptrs.clear();
    m_physicsScene.clearObjects();
}
