#include "view.h"

#include <QApplication>
#include <QKeyEvent>
#include <iostream>

#include "SceneBuilder.h"
#include "scene/Scene.h"


View::View(QWidget *parent)
: QOpenGLWidget(parent)
, m_time(), m_timer()
, m_isDragging(false)
, m_sceneBuilder()
, m_scene_ptr(nullptr)
{
    // View needs all mouse move events, not just mouse drag events
    setMouseTracking(true);

//    QApplication::setOverrideCursor(Qt::CrossCursor);

    // View needs keyboard focus
    setFocusPolicy(Qt::StrongFocus);

    // The update loop is implemented using a timer
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(tick()));
}

View::~View()
{
}

void View::initializeGL()
{
    // All OpenGL initialization *MUST* be done during or after this
    // method. Before this method is called, there is no active OpenGL
    // context and all OpenGL calls have no effect.

    //initialize glew
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if ( GLEW_OK != err ) {
        /* Problem: glewInit failed, something is seriously wrong. */
        std::cerr << "Something is very wrong, glew initialization failed." << std::endl;
    }
    std::cout << "Using GLEW " <<  glewGetString( GLEW_VERSION ) << std::endl;

    // Start a timer that will try to get 60 frames per second (the actual
    // frame rate depends on the operating system and other running programs)
    m_time.start();
    m_timer.start(1000 / 60);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    // create scene
    m_scene_ptr = std::make_unique<Scene>();
    m_sceneBuilder.initScene(*m_scene_ptr);

    this->settingsChanged();
}

void View::paintGL()
{
    // Get the number of seconds since the last tick (variable update rate)
    //float seconds = m_time.restart() * 0.001f;
    m_scene_ptr->render(this, m_time.restart());
}

void View::resizeGL(int w, int h)
{
    float ratio = static_cast<QGuiApplication *>(QCoreApplication::instance())->devicePixelRatio();
    w = static_cast<int>(w * ratio);
    h = static_cast<int>(h * ratio);
    glViewport(0, 0, w, h);
    m_scene_ptr->setScreenSize(w, h);
    m_scene_ptr->render(this);
}

glm::vec2 View::getMousePos(QMouseEvent *event) {
    // This starter code implements mouse capture, which gives the change in
    // mouse position since the last mouse movement. The mouse needs to be
    // recentered after every movement because it might otherwise run into
    // the edge of the screen, which would stop the user from moving further
    // in that direction. Note that it is important to check that deltaX and
    // deltaY are not zero before recentering the mouse, otherwise there will
    // be an infinite loop of mouse move events.

    int deltaX = event->x() - width() / 2;
    int deltaY = event->y() - height() / 2;

//    if (deltaX || deltaY)
//        QCursor::setPos(mapToGlobal(QPoint(width() / 2, height() / 2)));

//    auto w = static_cast<float>(width());
//    auto h = static_cast<float>(height());
    return {deltaX, deltaY};
}

void View::mousePressEvent(QMouseEvent *event){
    if (event->button() == Qt::RightButton) {
        auto delta = this->getMousePos(event);
        m_scene_ptr->mouseDown(delta.x, delta.y);
        m_isDragging = true;
        this->update();
    }
}

void View::mouseMoveEvent(QMouseEvent *event)
{
    if(m_isDragging) {
        auto delta = this->getMousePos(event);
        m_scene_ptr->mouseDragged(delta.x, delta.y);
        this->update();
    }
}

void View::mouseReleaseEvent(QMouseEvent *event) {
    if (m_isDragging && event->button() == Qt::RightButton) {
        auto delta = this->getMousePos(event);
        m_scene_ptr->mouseUp(delta.x, delta.y);
        m_isDragging = false;
        this->update();
    }
}

void View::wheelEvent(QWheelEvent *event) {
    m_scene_ptr->mouseScrolled(event->delta());
    this->update();
}


void View::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape) QApplication::quit();

    // TODO: Handle keyboard presses here
}

void View::keyReleaseEvent(QKeyEvent *event)
{
}


void View::tick()
{
    // Flag this view for repainting (Qt will call paintGL() soon after)
    this->update();
}

void View::createBall() {
    m_sceneBuilder.createBall(*m_scene_ptr);
    this->update();
}

void View::clearScene() {
    m_scene_ptr->clearLights();
    m_scene_ptr->clearObjects();
    m_sceneBuilder.initScene(*m_scene_ptr);
    this->update();
}

void View::settingsChanged() {
    m_sceneBuilder.setParametersFromSettings(*m_scene_ptr);
    m_sceneBuilder.applyBoxSettings();
    this->update();
}
