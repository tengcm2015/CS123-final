#include "SceneCamera.h"

SceneCamera::SceneCamera()
: m_aspectRatio (1)
, m_near        (1)
, m_far         (30)
, m_thetaH      (60)
, m_eye         { 2, 2, 2, 1}
, m_look        {-2,-2,-2, 0}
, m_up          { 0, 1, 0, 0}
{
    this->orientLook(m_eye, m_look, m_up);
}

glm::mat4x4 SceneCamera::getProjectionMatrix() const {
    return m_perspectiveTransformation * m_scaleMatrix;
}

glm::mat4x4 SceneCamera::getViewMatrix() const {
    return m_rotationMatrix * m_translationMatrix;
}

glm::mat4x4 SceneCamera::getScaleMatrix() const {
    return m_scaleMatrix;
}

glm::mat4x4 SceneCamera::getPerspectiveMatrix() const {
    return m_perspectiveTransformation;
}

glm::vec4 SceneCamera::getPosition() const {
    return m_eye;
}

glm::vec4 SceneCamera::getU() const {
    return m_u;
}

glm::vec4 SceneCamera::getV() const {
    return m_v;
}

glm::vec4 SceneCamera::getW() const {
    return m_w;
}

glm::vec4 SceneCamera::getLook() const {
    return -m_w;
}

float SceneCamera::getAspectRatio() const {
    return m_aspectRatio;
}

float SceneCamera::getHeightAngle() const {
    return m_thetaH;
}

void SceneCamera::orientLook(const glm::vec4 &eye, const glm::vec4 &look, const glm::vec4 &up) {
    m_eye = eye;
    m_w = -glm::normalize(look);
    m_v = glm::normalize(up - m_w * glm::dot(m_w, up));
    m_u = glm::vec4(glm::cross(glm::vec3(m_v), glm::vec3(m_w)), 0);

    this->updateViewMatrix();
    this->updateProjectionMatrix();
}

void SceneCamera::setHeightAngle(float h) {
    m_thetaH = h;
    this->updateProjectionMatrix();
}

void SceneCamera::setAspectRatio(float a) {
    m_aspectRatio = a;
    this->updateProjectionMatrix();
}

void SceneCamera::translate(const glm::vec4 &v) {
    m_eye += v;
    this->updateViewMatrix();
}

void SceneCamera::rotateU(float degrees) {
    float rads = glm::radians(degrees);
    float s = glm::sin(rads);
    float c = glm::cos(rads);

    glm::vec4 v = m_v * c + m_w * s;
    glm::vec4 w = -m_v * s + m_w * c;

    m_v = v;
    m_w = w;

    this->updateViewMatrix();
}

void SceneCamera::rotateV(float degrees) {
    float rads = glm::radians(degrees);
    float s = glm::sin(rads);
    float c = glm::cos(rads);

    glm::vec4 u = m_u * c - m_w * s;
    glm::vec4 w = m_u * s + m_w * c;

    m_u = u;
    m_w = w;

    this->updateViewMatrix();
}

void SceneCamera::rotateW(float degrees) {
    float rads = glm::radians(degrees);
    float s = glm::sin(rads);
    float c = glm::cos(rads);

    glm::vec4 v = m_v * c - m_u * s;
    glm::vec4 u = m_v * s + m_u * c;

    m_u = u;
    m_v = v;

    this->updateViewMatrix();
}

void SceneCamera::setClip(float nearPlane, float farPlane) {
    m_near = nearPlane;
    m_far = farPlane;
    this->updateProjectionMatrix();
}

void SceneCamera::updateProjectionMatrix() {
    this->updatePerspectiveMatrix();
    this->updateScaleMatrix();
}

void SceneCamera::updatePerspectiveMatrix() {
    float c = -m_near / m_far;

    m_perspectiveTransformation = glm::transpose(
        glm::mat4x4(1, 0, 0       , 0      ,
                    0, 1, 0       , 0      ,
                    0, 0, -1/(c+1), c/(c+1),
                    0, 0, -1      , 0      )
    );
}

void SceneCamera::updateScaleMatrix() {
    float fr = 1.f / m_far;
    float ftrh = fr / tan(glm::radians(m_thetaH / 2.f));
    float ftrw = ftrh / m_aspectRatio;

    // no need to transpose here because they are all same
    m_scaleMatrix = glm::mat4x4(
        ftrw, 0   , 0 , 0,
        0   , ftrh, 0 , 0,
        0   , 0   , fr, 0,
        0   , 0   , 0 , 1);
}

void SceneCamera::updateViewMatrix() {
    this->updateRotationMatrix();
    this->updateTranslationMatrix();
}

void SceneCamera::updateRotationMatrix() {
    m_rotationMatrix = glm::transpose(
        glm::mat4x4(m_u.x, m_u.y, m_u.z, 0,
                    m_v.x, m_v.y, m_v.z, 0,
                    m_w.x, m_w.y, m_w.z, 0,
                    0    , 0    , 0    , 1)
    );
}

void SceneCamera::updateTranslationMatrix() {
    m_translationMatrix = glm::transpose(
        glm::mat4x4(1, 0, 0, -m_eye.x,
                    0, 1, 0, -m_eye.y,
                    0, 0, 1, -m_eye.z,
                    0, 0, 0, 1       )
    );
}

