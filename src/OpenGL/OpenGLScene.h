#ifndef TP_IG_M2_S2_OPENGLSCENE_H
#define TP_IG_M2_S2_OPENGLSCENE_H

#include "../Scene/Scene.h"
#include "../Scene/AbstractLight.h"
#include "OpenGLMesh.h"
#include "OpenGLEnvironment.h"
#include "OpenGLUniformBufferObject.h"

class OpenGLScene : public QObject {
    Q_OBJECT
public:
    explicit OpenGLScene(Scene* scene);

    ~OpenGLScene() override;

    Scene *host() const;

    void renderModels();
    void renderLights();
    void renderEnvironment();

    void commitCameraInfo();
    void commitLightInfo(AbstractLight *light);

    bool hasEnvironment();

private slots:
    void modelAdded(Model* model);
    void meshAdded(Mesh* mesh);
    void lightAdded(AbstractLight* light);
    void environmentChanged(Environment *environment);

private:
    Scene* _host;
    QVector<OpenGLMesh*> _normalMeshes;
    QVector<OpenGLMesh*> _lightMeshes;
    std::unique_ptr<OpenGLEnvironment> _environment;

    static OpenGLUniformBufferObject *_cameraInfo, *_lightInfo;
};


#endif //TP_IG_M2_S2_OPENGLSCENE_H
