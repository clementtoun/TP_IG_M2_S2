#ifndef TP_IG_M2_S2_MAINWINDOW_H
#define TP_IG_M2_S2_MAINWINDOW_H

#include <QMainWindow>
#include "OpenGL/OpenGLWidget.h"
#include "Scene/Scene.h"

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);

    ~MainWindow() override;

private slots:
    void fileImportModel();
    void fileImportEnvironment();

private:
    Scene *_host;
    OpenGLWidget *_openGLWidget;

};


#endif //TP_IG_M2_S2_MAINWINDOW_H
