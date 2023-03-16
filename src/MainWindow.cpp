#include <QFileDialog>
#include <QMenu>
#include <QMenuBar>
#include <QSurfaceFormat>
#include <QTimer>
#include <QDateTime>
#include <QScrollArea>
#include <iostream>
#include <QStackedLayout>
#include "MainWindow.h"
#include "Scene/ModelLoader.h"
#include "Scene/EnvironmentLoader.h"

MainWindow::MainWindow(QWidget * parent) : QMainWindow(parent), _host(0) {


    _host = new Scene();
    _renderer = new OpenGLRenderer();
    _openGLWidget = new OpenGLWidget(new OpenGLScene(_host), _renderer);

    setCentralWidget(_openGLWidget);
    _openGLWidget->setFocusPolicy(Qt::StrongFocus);

    QMenu *menuFile = menuBar()->addMenu("File");
    menuFile->addAction("Import Model", this, SLOT(fileImportModel()));
    menuFile->addAction("Import Environment", this, SLOT(fileImportEnvironment()));

    QMenu *menuSetting = menuBar()->addMenu("Option");
    _shadowActive = new QAction("Shadow", this);
    _shadowActive->setCheckable(true);
    _shadowActive->setChecked(true);
    connect(_shadowActive, SIGNAL(toggled(bool)), this, SLOT(activeShadow(bool)));
    menuSetting->addAction(_shadowActive);

    _dock = new QDockWidget(this);
    _sceneTree = new SceneTreeWidget(_host, this);
    _dock->setWidget(_sceneTree);
    addDockWidget(Qt::LeftDockWidgetArea, _dock);

    _dockTools = new QDockWidget(this);
    _scrollTools = new QScrollArea(_dockTools);
    _dockTools->setWidget(_scrollTools);
    addDockWidget(Qt::RightDockWidgetArea, _dockTools);

    //_host->addPointLight(new PointLight(QVector3D(1.0,1.0,1.0),QVector3D(-15., 3., 0.)));
    //_host->addPointLight(new PointLight(QVector3D(1.0,1.0,1.0),QVector3D(5., 3.5, 0.)));
    _host->addPointLight(new PointLight(QVector3D(1.0,1.0,1.0),QVector3D(0.,1., 0.)));
    _host->addDirectionalLight(new DirectionalLight({1,1,1},QVector3D(0.0f, -1.f, 0.1f)));

    connect(_sceneTree, SIGNAL(itemSelected(QVariant)), this, SLOT(itemSelected(QVariant)));

    _timer = new QTimer(this);
    connect(_timer, SIGNAL(timeout()), this, SLOT(animate()));
    _timer->start(10);
}


void MainWindow::fileImportModel() {
    QString filePath = QFileDialog::getOpenFileName(this, "Load Model", "../Models/", "Mesh (*.gltf *.glb *.obj)");
    if (filePath == 0) return;

    ModelLoader loader;
    _host->addModel(loader.loadModelFromFile(filePath));
    _openGLWidget->update();
}

void MainWindow::fileImportEnvironment() {
    QString filePath = QFileDialog::getOpenFileName(this, "Load Environment", "../Environments/", "Environment (*.jpg *.png)");
    if (filePath == 0) return;

    EnvironmentLoader envLoader;
    _host->changeEnvironment(envLoader.loadFromFile(filePath));
    _openGLWidget->update();
}

MainWindow::~MainWindow() {
    delete _host;
    delete _openGLWidget;
}

void MainWindow::animate() {

    float dt;

    if(_last_time != 0)
        dt = (float) (QDateTime::currentMSecsSinceEpoch() - _last_time) / 1000.f;
    else
        dt = 0;

    _host->updatePosition(dt);

    _last_time = QDateTime::currentMSecsSinceEpoch();

    _openGLWidget->update();
}

void MainWindow::itemSelected(QVariant item) {
    std::cout << "click" << std::endl;
    delete _scrollTools->takeWidget();

    if (item.canConvert<Model*>()) {
        _scrollTools->setWidget(new ModelProperty(item.value<Model*>(), this));
    } else if (item.canConvert<Mesh*>()) {
        //_scrollTools->setWidget(new MeshProperty(item.value<Mesh*>(), this));
    }
}

void MainWindow::itemDeselected(QVariant item) {

}

void MainWindow::activeShadow(bool value) {
    _renderer->setShadowActive(value);
}
