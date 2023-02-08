#ifndef TP_IG_M2_S2_SCENE_H
#define TP_IG_M2_S2_SCENE_H

#include "Model.h"
#include "Camera.h"
#include "PointLight.h"
#include "AbstractLight.h"
#include "Environment.h"

class Scene : public QObject {
    Q_OBJECT

public:
    Scene();

    ~Scene() override;

    bool addModel(Model *model);
    bool addPointLight(PointLight *pointLight);

    bool changeEnvironment(Environment* environment);

    Camera *camera() const;
    bool changeCamera();

    const QVector<PointLight *> &pointLights() const;

signals:
    void modelAdded(Model* model);
    void lightAdded(AbstractLight* light);
    void cameraChanged(Camera* camera);
    void environmentChanged(Environment *environment);

private:
    QVector<Model*> _models;
    QVector<PointLight*> _pointLights;

    std::unique_ptr<Environment> _environment;

    std::unique_ptr<Camera> _camera;
    using cameraconstructor=std::function<Camera*(QVector3D, QVector3D, float, float, float)>;
    std::vector<cameraconstructor> _cameraconstructors;
    unsigned int m_active_camera;
};


#endif //TP_IG_M2_S2_SCENE_H
