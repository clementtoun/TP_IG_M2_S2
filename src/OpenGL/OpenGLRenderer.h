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

    void setShadowCascadeLevels(float cameraFarPlane);
    QVector<QMatrix4x4> getLightSpaceMatrices(Camera& camera, const QVector3D& lightDir);
    void initialiseShadowFrameBuffer();
    void initialiseHDR();

    void setSize(unsigned int width, unsigned height);

    void setContext(QOpenGLContext *context);

    bool isShadowActive() const;

    void setShadowActive(bool shadowActive);

private:
    QOpenGLContext* _context;

    QOpenGLShaderProgram *_gltfShader;
    QOpenGLShaderProgram *_cubeMapShader;
    QOpenGLShaderProgram *_lightShader;
    QOpenGLShaderProgram *_CSMShader;
    QOpenGLShaderProgram *_shadowCubeShader;
    QOpenGLShaderProgram *_hdrShader;
    unsigned int _depthMapFBO;
    unsigned int _depthCSMap;
    unsigned int _depthCubeMap;

    QVector<float> _shadowCascadeLevels;

    unsigned int _colorBuffer;
    unsigned int _hdrFBO;
    unsigned int _rboDepth;
    unsigned int _quadVAO = 0;
    unsigned int _quadVBO = 0;

    QOpenGLFunctions_4_5_Core *glFuncs;
    const unsigned int SHADOW_WIDTH = 2048, SHADOW_HEIGHT = 2048;
    unsigned int _width, _height;

    bool _shadowActive;
};


#endif //TP_IG_M2_S2_OPENGLRENDERER_H
