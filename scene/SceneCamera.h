#ifndef SCENECAMERA_H
#define SCENECAMERA_H

#include "glm/glm.hpp"

class SceneCamera {
public:
    // Initialize your camera.
    SceneCamera();

    // Sets the aspect ratio of this camera. Automatically called by the GUI when the window is
    // resized.
    void setAspectRatio(float aspectRatio);

    // Returns the projection matrix given the current camera settings.
    glm::mat4x4 getProjectionMatrix() const;

    // Returns the view matrix given the current camera settings.
    glm::mat4x4 getViewMatrix() const;

    // Returns the matrix that scales down the perspective view volume into the canonical
    // perspective view volume, given the current camera settings.
    glm::mat4x4 getScaleMatrix() const;

    // Returns the matrix the unhinges the perspective view volume, given the current camera
    // settings.
    glm::mat4x4 getPerspectiveMatrix() const;

    // Returns the current position of the camera.
    glm::vec4 getPosition() const;

    // Returns the current u vector for this camera.
    glm::vec4 getU() const;

    // Returns the current v vector for this camera.
    glm::vec4 getV() const;

    // Returns the current w vector for this camera.
    glm::vec4 getW() const;

    // Returns the current look vector for this camera.
    glm::vec4 getLook() const;

    // Returns the currently set aspect ratio.
    float getAspectRatio() const;

    // Returns the currently set height angle.
    float getHeightAngle() const;

    // Move this camera to a new eye position, and orient the camera's axes given look and up
    // vectors.
    void orientLook(const glm::vec4 &eye, const glm::vec4 &look, const glm::vec4 &up);

    // Sets the height angle of this camera.
    void setHeightAngle(float h);

    // Translates the camera along a given vector.
    void translate(const glm::vec4 &v);

    // Rotates the camera about the U axis by a specified number of degrees.
    void rotateU(float degrees);

    // Rotates the camera about the V axis by a specified number of degrees.
    void rotateV(float degrees);

    // Rotates the camera about the W axis by a specified number of degrees.
    void rotateW(float degrees);

    // Sets the near and far clip planes for this camera.
    void setClip(float nearPlane, float farPlane);

private:
    float m_aspectRatio;
    float m_near, m_far;

    float m_thetaH; // m_thetaW will be calculated from aspect ratio
    glm::vec4 m_eye, m_look, m_up;
    glm::vec4 m_u, m_v, m_w;

    glm::mat4 m_scaleMatrix, m_perspectiveTransformation;
    glm::mat4 m_translationMatrix, m_rotationMatrix;

    void updateProjectionMatrix();
    void updatePerspectiveMatrix();
    void updateScaleMatrix();
    void updateViewMatrix();
    void updateRotationMatrix();
    void updateTranslationMatrix();

};

#endif // SCENECAMERA_H
