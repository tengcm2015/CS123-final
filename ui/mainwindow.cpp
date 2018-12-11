#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "view.h"
#include "Databinding.h"
#include "Settings.h"
#include <QGLFormat>

#define BIND(b) { \
    DataBinding *_b = (b); \
    m_bindings.push_back(_b); \
    assert(connect(_b, SIGNAL(dataChanged()), this, SLOT(settingsChanged()))); \
}

MainWindow::MainWindow(QWidget *parent)
: QMainWindow(parent)
, ui(new Ui::MainWindow)
{
    // Make sure the settings are loaded before the UI
    settings.loadSettingsOrDefaults();

    ui->setupUi(this);

    // data bindings

    // createBall/GL/color
    BIND(UCharBinding::bindSliderAndTextbox(
            ui->sliderR, ui->valueR, settings.ballColor.r, 0, 255))
    BIND(UCharBinding::bindSliderAndTextbox(
            ui->sliderG, ui->valueG, settings.ballColor.g, 0, 255))
    BIND(UCharBinding::bindSliderAndTextbox(
            ui->sliderB, ui->valueB, settings.ballColor.b, 0, 255))


    // createBall/GL/textures
    QButtonGroup *texturesButtonGroup = new QButtonGroup;

    BIND(ChoiceBinding::bindRadioButtons(
            texturesButtonGroup,
            NUM_TEXTURE_TYPES,
            settings.textureType,
            ui->radioTextureNone,
            ui->radioTextureMetal,
            ui->radioTextureMoss))

    // createBall/GL/shininess
    BIND(FloatBinding::bindSliderAndTextbox(
            ui->sliderShininess, ui->valueShininess, settings.shininess, 1.f, 100.f))

    // createBall/GL/transparency
    BIND(FloatBinding::bindSliderAndTextbox(
            ui->sliderTransparency, ui->valueTransparency, settings.transparency, 0.1f, 10.f))

    // createBall/physics/velocity
    BIND(FloatBinding::bindSliderAndTextbox(
            ui->sliderVelocityX, ui->valueVelocityX, settings.velocity.x, -10.f, 10.f))
    BIND(FloatBinding::bindSliderAndTextbox(
            ui->sliderVelocityY, ui->valueVelocityY, settings.velocity.y, -10.f, 10.f))
    BIND(FloatBinding::bindSliderAndTextbox(
            ui->sliderVelocityZ, ui->valueVelocityZ, settings.velocity.z, -10.f, 10.f))

    // createBall/physics/density
    BIND(FloatBinding::bindSliderAndTextbox(
            ui->sliderDensity, ui->valueDensity, settings.density, 0.1f, 10.f))

    // createBall/physics/restitution
    BIND(FloatBinding::bindSliderAndTextbox(
            ui->sliderRestitution, ui->valueRestitution, settings.restitution, 0.1f, 2.f))

    // createBall/physics/friction
    BIND(FloatBinding::bindSliderAndTextbox(
            ui->sliderFriction, ui->valueFriction, settings.friction, 1.f, 100.f))

    // createBall/radius
    BIND(FloatBinding::bindSliderAndTextbox(
            ui->sliderRadius, ui->valueRadius, settings.radius, 0.1f, 1.f))

    // sceneSettings/gravity
    BIND(FloatBinding::bindSliderAndTextbox(
            ui->sliderGravityX, ui->valueGravityX, settings.gravity.x, -10.f, 10.f))
    BIND(FloatBinding::bindSliderAndTextbox(
            ui->sliderGravityY, ui->valueGravityY, settings.gravity.y, -10.f, 10.f))
    BIND(FloatBinding::bindSliderAndTextbox(
            ui->sliderGravityZ, ui->valueGravityZ, settings.gravity.z, -10.f, 10.f))

    // sceneSettings/features
    BIND(BoolBinding::bindCheckbox(ui->togglerRaymarching, settings.useRaymarching))
    BIND(BoolBinding::bindCheckbox(ui->togglerFXAA, settings.useFXAA))
    BIND(BoolBinding::bindCheckbox(ui->togglerCollision, settings.enableCollision))
}

MainWindow::~MainWindow()
{
    foreach (DataBinding *b, m_bindings)
        delete b;
    foreach (QButtonGroup *bg, m_buttonGroups)
        delete bg;
    delete ui;
}

void MainWindow::settingsChanged() {
    ui->view->settingsChanged();
}
