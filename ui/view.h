#ifndef VIEW_H
#define VIEW_H

#include "GL/glew.h"
#include <memory>
#include <QOpenGLWidget>
#include <QTime>
#include <QTimer>
#include "scene/Scene.h"

class View : public QOpenGLWidget {
    Q_OBJECT
public:
    View(QWidget *parent);
    ~View();

private:
    QTime m_time;
    QTimer m_timer;
    bool m_captureMouse;

    std::unique_ptr<Scene> m_scene_ptr;

    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int w, int h) override;

    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

private slots:
    void tick();
};

#endif // VIEW_H
