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

    void initialiseShadowFrameBuffer();

    void setWidth(unsigned int width);
    void setHeight(unsigned int height);

    void setContext(QOpenGLContext *context);

private:
    QOpenGLContext* _context;

    QOpenGLShaderProgram *_basicShader;
    QOpenGLShaderProgram *_cubeMapShader;
    QOpenGLShaderProgram *_lightShader;
    QOpenGLShaderProgram *_shadowMapShader;
    QOpenGLShaderProgram *_shadowCubeShader;
    unsigned int _depthMapFBO;
    unsigned int _depthMap;
    unsigned int _depthCubeMap;
    QOpenGLFunctions_3_3_Core *glFuncs;
    const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
    unsigned int _width, _height;
};


#endif //TP_IG_M2_S2_OPENGLRENDERER_H
