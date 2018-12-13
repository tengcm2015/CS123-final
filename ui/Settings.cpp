#include "Settings.h"
#include <QFile>
#include <QSettings>

Settings settings;


/**
 * Loads the application settings, or, if no saved settings are available, loads default values for
 * the settings. You can change the defaults here.
 */
void Settings::loadSettingsOrDefaults() {
    // Set the default values below
    QSettings s("CS123", "FinalProject");

    // createBall/GL/colors
    ballColor.r = s.value("ballRed", 255).toInt();
    ballColor.g = s.value("ballGreen", 127).toInt();
    ballColor.b = s.value("ballBlue", 0).toInt();

    // createBall/GL/textures
    textureType = s.value("textureType", TEXTURE_NONE).toInt();

    // createBall/GL/shininess
    shininess = s.value("shininess", 64.f).toDouble();

    // createBall/GL/transparency
    transparency = s.value("transparency", 0).toDouble();

    // createBall/physics/velocity
    velocity.x = s.value("velocityX", 0).toDouble();
    velocity.y = s.value("velocityY", 0).toDouble();
    velocity.z = s.value("velocityZ", 0).toDouble();

    // createBall/physics/density
    density = s.value("density", 1.f).toDouble();

    // createBall/physics/restitution
    restitution = s.value("restitution", 1.f).toDouble();

    // createBall/physics/friction
    friction = s.value("friction", 1.f).toDouble();

    // createBall/radius
    radius = s.value("radius", .25f).toDouble();

    // sceneSettings/boxTexture
    boxTextureType = s.value("boxTextureType", TEXTURE_NONE).toInt();

    // sceneSettings/gravity
    gravity.x = s.value("gravityX", 0).toDouble();
    gravity.y = s.value("gravityY", -1).toDouble();
    gravity.z = s.value("gravityZ", 0).toDouble();

    // sceneSettings/features
    useRaymarching = false; // force value to false for now
//    useRaymarching = s.value("useRaymarching", false).toBool();
    useFXAA = s.value("useFXAA", true).toBool();
    enableCollision = s.value("enableCollision", true).toBool();
}

void Settings::saveSettings() {
    QSettings s("CS123", "FinalProject");

    // createBall/GL/colors
    s.setValue("ballRed", ballColor.r);
    s.setValue("ballGreen", ballColor.g);
    s.setValue("ballBlue", ballColor.b);

    // createBall/GL/textures
    s.setValue("textureType", textureType);

    // createBall/GL/shininess
    s.setValue("shininess", shininess);

    // createBall/GL/transparency
    s.setValue("transparency", transparency);

    // createBall/physics/velocity
    s.setValue("velocityX", velocity.x);
    s.setValue("velocityY", velocity.y);
    s.setValue("velocityZ", velocity.z);

    // createBall/physics/density
    s.setValue("density", density);

    // createBall/physics/restitution
    s.setValue("restitution", restitution);

    // createBall/physics/friction
    s.setValue("friction", friction);

    // createBall/radius
    s.setValue("radius", radius);

    // sceneSettings/gravity
    s.setValue("gravityX", gravity.x);
    s.setValue("gravityY", gravity.y);
    s.setValue("gravityZ", gravity.z);

    // sceneSettings/boxTexture
    s.setValue("boxTextureType", boxTextureType);

    // sceneSettings/features
    s.setValue("useRaymarching", useRaymarching);
    s.setValue("useFXAA", useFXAA);
    s.setValue("enableCollision", enableCollision);
}
