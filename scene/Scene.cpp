#include "Scene.h"
#include "ui/view.h"
#include "ui/Settings.h"
#include "lib/ResourceLoader.h"
#include "gl/textures/Texture2D.h"

#include <sstream>
#include "glm/gtx/transform.hpp"  // glm::translate, scale, rotate

using namespace CS123::GL;
using namespace CS123::PHYSICS;

Scene::Scene()
: m_width(0), m_height(0)
, m_camera()
, m_cam_angleX(45), m_cam_angleY(45)
, m_zoomZ(-4)
, m_gravity(0.0f)
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

    this->updateSceneCamera();
    m_gravityCoord_mat = m_camera.getViewMatrix();
}

Scene::~Scene()
{
}


SceneLightData Scene::alignLightWithCamera(const SceneLightData &l) {
    SceneLightData lightData = l;
    lightData.dir = glm::inverse(m_camera.getViewMatrix()) * l.dir;
    return lightData;
}

void Scene::setRaymarchSceneUniforms() {
    m_renderShader->setUniform("eye" , m_camera.getPosition().xyz());
    m_renderShader->setUniform("look", m_camera.getLook().xyz());
    m_renderShader->setUniform("up"  , m_camera.getV().xyz());
    m_renderShader->setUniform("aspectRatio", m_camera.getAspectRatio());

    for (int i = 0; i < RayMarchShader::MAX_NUM_LIGHTS; ++i)
        m_renderShader->clearLight(i);
    for (int i = 0; i < m_sceneLights.size(); ++i)
        m_renderShader->setLight(alignLightWithCamera(m_sceneLights[i]), i);

    for (int i = 0; i < RayMarchShader::MAX_PRIMITIVES; ++i)
        m_renderShader->clearPrimitive(i);
    for (int i = 0; i < m_primitive_ptrs.size(); ++i)
        m_renderShader->setPrimitive(m_primitive_ptrs[i], i);
}

void Scene::setPhongSceneUniforms() {
    m_phongShader->setUniform("p", m_camera.getProjectionMatrix());
    m_phongShader->setUniform("v", m_camera.getViewMatrix());

    m_phongShader->setUniform("useLighting", true);
    m_phongShader->setUniform("useArrowOffsets", false);

    for (int i = 0; i < PhongShader::MAX_NUM_LIGHTS; ++i)
        m_phongShader->clearLight(i);
    for (int i = 0; i < m_sceneLights.size(); ++i)
        m_phongShader->setLight(alignLightWithCamera(m_sceneLights[i]), i);
}

void Scene::__render() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (settings.useRaymarching) {
        m_renderShader->bind();
        setRaymarchSceneUniforms();
        m_fullScreenQuad.draw();
        m_renderShader->unbind();

    } else {
        m_phongShader->bind();
        setPhongSceneUniforms();
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


void Scene::setScreenSize(int w, int h) {
    m_width = w, m_height = h;
    m_camera.setAspectRatio(static_cast<float>(w) / static_cast<float>(h));
    m_tmp_FBO = std::make_unique<FBO>(1, FBO::DEPTH_STENCIL_ATTACHMENT::DEPTH_ONLY, w, h,
                                       TextureParameters::WRAP_METHOD::CLAMP_TO_EDGE);
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
    return primitive_ptr;
}


void Scene::addLight(const SceneLightData &sceneLight) {
    m_sceneLights.push_back(sceneLight);
}

void Scene::setGlobal(const SceneGlobalData &global) {
    m_globalData = global;
}

void Scene::setGravity(glm::vec3 gravity) {
    m_gravity = gravity;
    this->updateGravity();
}

void Scene::clearLights() {
    m_sceneLights.clear();
}

void Scene::clearObjects() {
    m_object_ptrs.clear();
    m_primitive_ptrs.clear();
    m_physicsScene.clearObjects();
}


void Scene::updateSceneCamera() {
    auto eye = glm::rotate(glm::radians(m_cam_angleY), glm::vec3(0.f, 1.f, 0.f))
             * glm::rotate(glm::radians(m_cam_angleX), glm::vec3(1.f, 0.f, 0.f))
             * glm::translate(glm::vec3(0.f, 0.f, m_zoomZ))
             * glm::vec4(0, 0, 0, 1);

    auto look = glm::vec4(-eye.xyz(), 0);
    auto up = glm::vec4(0, 1, 0, 0);

    m_camera.orientLook(eye, look, up);
}

void Scene::updateGravity() {
    auto desc = m_physicsScene.getGlobal();
    auto g = glm::vec4(m_gravity, 0);
    auto cam2grav = glm::inverse(m_camera.getViewMatrix()) * m_gravityCoord_mat;
    desc.gravity = (glm::length(m_gravity) * cam2grav * glm::normalize(g)).xyz();
    m_physicsScene.setGlobal(desc);
}


void Scene::mouseDown(float deltaX, float deltaY) {
    oldX = deltaX;
    oldY = deltaY;
}

void Scene::mouseDragged(float deltaX, float deltaY) {
    m_cam_angleY -= deltaX - oldX;
    m_cam_angleX += deltaY - oldY;
    oldX = deltaX;
    oldY = deltaY;

    if (m_cam_angleX < -90) m_cam_angleX = -90;
    if (m_cam_angleX > 90) m_cam_angleX = 90;

    this->updateSceneCamera();
    this->updateGravity();
}

void Scene::mouseUp(float deltaX, float deltaY) {
}

void Scene::mouseScrolled(float delta) {
    // Use an exponential factor so the zoom increments are small when we are
    // close to the object and large when we are far away from the object
    m_zoomZ *= powf(0.999f, delta);

    this->updateSceneCamera();
}
