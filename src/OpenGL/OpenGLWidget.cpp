#include "OpenGLWidget.h"
#include <QMouseEvent>
#include <QDateTime>
#include <QtMath>

OpenGLWidget::OpenGLWidget() {
    _scene = nullptr;
    _renderer = nullptr;
    _drawFill = true;
}

OpenGLWidget::OpenGLWidget(OpenGLScene *openGLScene, OpenGLRenderer *renderer) {
    _scene = openGLScene;
    _renderer = renderer;

    QSurfaceFormat format;
    format.setMajorVersion(6); format.setMinorVersion(4);
    format.setProfile(QSurfaceFormat::CoreProfile);
    format.setSamples(16);
    //format.setColorSpace(QSurfaceFormat::sRGBColorSpace);
    setFormat(format);
    //setTextureFormat(GL_SRGB);
    _drawFill = true;
}

void OpenGLWidget::initializeGL() {
    initializeOpenGLFunctions();
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_CULL_FACE);

    if (_renderer) {
        _renderer->setContext(context());
        _renderer->setSize(width(), height());
        _renderer->reloadShaders();
        _renderer->setShadowCascadeLevels(_scene->host()->camera()->getFar());
        _renderer->initialiseShadowFrameBuffer();
        _renderer->initialiseHDR();
    }
}

void OpenGLWidget::resizeGL(int w, int h) {
    glViewport(0, 0, width(), height());
    _renderer->setSize(width(), height());
}

void OpenGLWidget::paintGL() {
    glClearColor(0.082, 0.297, 0.473, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if(_drawFill)
        glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
    else
        glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

    if (_renderer && _scene && _scene->host()->camera()) {
        _scene->host()->camera()->setAspect(float(width()) / float(height()));
        _scene->commitCameraInfo();
        _renderer->render(_scene);
    }

    _last_time = QDateTime::currentMSecsSinceEpoch();
}

void OpenGLWidget::keyPressEvent(QKeyEvent *event) {
    switch (event->key()) {
        case Qt::Key_C:
        {
            _scene->host()->changeCamera();
            break;
        }
        case Qt::Key_B:
        {
            toggleDrawFill();
            break;
        }
    }
    update();
}

void OpenGLWidget::mousePressEvent(QMouseEvent *event) {
    _scene->host()->camera()->processmouseclick(event->button(), (float) event->x(), (float) event->y());
}


void OpenGLWidget::wheelEvent(QWheelEvent *event) {
    QPoint numDegrees = event->angleDelta();
    _scene->host()->camera()->processmousescroll((float) numDegrees.y() / 8);
    update();
}

void OpenGLWidget::mouseMoveEvent(QMouseEvent *event) {
    _scene->host()->camera()->processmousemovement(1, (float) event->x(), (float) event->y());
    update();
}

OpenGLWidget::~OpenGLWidget() {
    makeCurrent();
    delete _scene;
    delete _renderer;
}

void OpenGLWidget::toggleDrawFill() {
    _drawFill = !_drawFill;
}


