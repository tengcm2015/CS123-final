/**
 * @file    Settings.h
 *
 * This file contains various settings and enumerations that you will need to use in the various
 * assignments. The settings are bound to the GUI via static data bindings.
 */

#ifndef SETTINGS_H
#define SETTINGS_H

#include "lib/BGRA.h"
#include "scene/SceneData.h"
#include "physics/PhysicsData.h"

// Enumeration values for the Filters that the user can select in the GUI.
enum TextureType {
    TEXTURE_NONE,
    TEXTURE_SMOOTH,
    TEXTURE_PATTERNED,
    NUM_TEXTURE_TYPES
};

struct Settings {
    // createBall/GL/colors
    BGRA ballColor;

    // createBall/GL/textures
    int textureType;

    // createBall/GL/shininess
    float shininess;

    // createBall/GL/transparency
    float transparency;

    // createBall/physics/velocity
    glm::vec3 velocity;

    // createBall/physics/density
    float density;

    // createBall/physics/restitution
    float restitution;

    // createBall/physics/friction
    float friction;

    // createBall/radius
    float radius;

    // sceneSettings/gravity
    glm::vec3 gravity;

    // sceneSettings/boxTexture
    int boxTextureType;

    // sceneSettings/features
    bool useRaymarching;
    bool useFXAA;
    bool enableCollision;

    void loadSettingsOrDefaults();
    void saveSettings();
};

// The global Settings object, will be initialized by MainWindow
extern Settings settings;

#endif // SETTINGS_H
