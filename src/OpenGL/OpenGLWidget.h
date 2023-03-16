#ifndef TP_IG_M2_S2_OPENGLWIDGET_H
#define TP_IG_M2_S2_OPENGLWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions_4_5_Core>
#include "OpenGLScene.h"
#include "OpenGLRenderer.h"


class OpenGLWidget : public QOpenGLWidget, protected QOpenGLFunctions_4_5_Core {
    Q_OBJECT

public:
    OpenGLWidget();
    OpenGLWidget(OpenGLScene* openGLScene, OpenGLRenderer* renderer);

    ~OpenGLWidget() override;

    void toggleDrawFill();

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private:
    OpenGLScene* _scene;
    OpenGLRenderer* _renderer;

    bool _drawFill;

    qint64 _last_time = 0;
};


#endif //TP_IG_M2_S2_OPENGLWIDGET_H
