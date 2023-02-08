#ifndef TP_IG_M2_S2_OPENGLRENDERER_H
#define TP_IG_M2_S2_OPENGLRENDERER_H


#include <QObject>
#include <QOpenGLShaderProgram>
#include "OpenGLScene.h"

class OpenGLRenderer : public QObject {
    Q_OBJECT

public:
    explicit OpenGLRenderer(QObject* parent = 0);

    ~OpenGLRenderer() override;

    bool reloadShaders();

    void render(OpenGLScene* openGLScene);

private:
    QOpenGLShaderProgram *_basicShader;
    QOpenGLShaderProgram *_cubeMapShader;
};


#endif //TP_IG_M2_S2_OPENGLRENDERER_H
