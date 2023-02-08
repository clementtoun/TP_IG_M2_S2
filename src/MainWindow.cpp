#include <QFileDialog>
#include <QMenu>
#include <QMenuBar>
#include <QSurfaceFormat>
#include "MainWindow.h"
#include "Scene/ModelLoader.h"
#include "Scene/EnvironmentLoader.h"

MainWindow::MainWindow(QWidget * parent) : QMainWindow(parent), _host(0) {

    _host = new Scene();
    _openGLWidget = new OpenGLWidget(new OpenGLScene(_host), new OpenGLRenderer());

    setCentralWidget(_openGLWidget);
    _openGLWidget->setFocusPolicy(Qt::StrongFocus);

    QMenu *menuFile = menuBar()->addMenu("File");
    menuFile->addAction("Import Model", this, SLOT(fileImportModel()));
    menuFile->addAction("Import Environment", this, SLOT(fileImportEnvironment()));

    _host->addPointLight(new PointLight(QVector3D(1.0,1.0,1.0),QVector3D(1., 2., 0.)));
}


void MainWindow::fileImportModel() {
    QString filePath = QFileDialog::getOpenFileName(this, "Load Model", "../Models/", "Mesh (*.gltf *.glb *.obj)");
    if (filePath == 0) return;

    ModelLoader loader;
    _host->addModel(loader.loadModelFromFile(filePath));
    _openGLWidget->update();
}

void MainWindow::fileImportEnvironment() {
    QString filePath = QFileDialog::getOpenFileName(this, "Load Environment", "../Environments/", "Environment (*.jpg)");
    if (filePath == 0) return;

    EnvironmentLoader envLoader;
    _host->changeEnvironment(envLoader.loadFromFile(filePath));
    _openGLWidget->update();
}

MainWindow::~MainWindow() {
    delete _host;
    delete _openGLWidget;
}
