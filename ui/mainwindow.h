#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <memory>

#include <QButtonGroup>
#include <QMainWindow>

class View;

namespace Ui {
    class MainWindow;
}

class DataBinding;

/**
 * @class MainWindow
 *
 * The main graphical user interface class (GUI class) for our application.
 */
class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    // Used internally to keep data bindings and the user interface in sync.
    void settingsChanged();

private:
    // Auto-generated by Qt. DO NOT RENAME!
    Ui::MainWindow *ui;

    QList<DataBinding*> m_bindings;
    QList<QButtonGroup*> m_buttonGroups;
};

#endif // MAINWINDOW_H
