#ifndef TP_IG_M2_S2_MAINWINDOW_H
#define TP_IG_M2_S2_MAINWINDOW_H

#include <QMainWindow>
#include <QDockWidget>
#include <QScrollArea>
#include "OpenGL/OpenGLWidget.h"
#include "Scene/Scene.h"
#include "SceneTreeWidget.h"

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);

    ~MainWindow() override;

private slots:
    void fileImportModel();
    void fileImportEnvironment();
    void animate();
    void itemSelected(QVariant item);
    void itemDeselected(QVariant item);
    void activeShadow(bool value);

private:
    Scene *_host;
    OpenGLRenderer* _renderer;
    OpenGLWidget *_openGLWidget;
    QDockWidget *_dock;
    SceneTreeWidget *_sceneTree;
    QTimer* _timer;
    QDockWidget* _dockTools;
    QScrollArea *_scrollTools;
    qint64 _last_time = 0;
    QAction* _shadowActive;
};


#endif //TP_IG_M2_S2_MAINWINDOW_H
