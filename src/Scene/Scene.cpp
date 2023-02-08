
#include <iostream>
#include "Scene.h"

Scene::Scene() {
    setObjectName("Untilted Scene");

    _cameraconstructors.emplace_back([](QVector3D position, QVector3D up, float yaw, float pitch, float fov)->TrackballCamera*{
        std::cout << "TrackballCamera loaded" << std::endl; return new TrackballCamera(position, up, yaw, pitch, fov);
    });
    _cameraconstructors.emplace_back([](QVector3D position, QVector3D up, float yaw, float pitch, float fov)->EulerCamera*{
        std::cout << "EulerCamera loaded" << std::endl; return new EulerCamera(position, up, yaw, pitch, fov);
    });
    _camera.reset(_cameraconstructors[0](QVector3D(0.0f,0.0f,10.0f), QVector3D(0.0,1.0,0.0), -90.0, 0, 45.0));
    m_active_camera = 0;
}

Scene::~Scene() {
    for(auto m : _models)
        delete m;
    _models.clear();
    for(auto pl : _pointLights)
        delete pl;
    _pointLights.clear();
    _cameraconstructors.clear();
}

bool Scene::addModel(Model *model) {
    if (_models.contains(model))
        return false;

    _models.push_back(model);
    model->setParent(this);
    modelAdded(model);

    return true;
}

bool Scene::addPointLight(PointLight *pointLight) {
    if(_pointLights.contains(pointLight))
        return false;

    _pointLights.push_back(pointLight);
    pointLight->setParent(this);
    lightAdded(pointLight);

    return true;
}

bool Scene::changeEnvironment(Environment *environment) {
    _environment.reset(environment);
    environmentChanged(environment);

    return true;
}

bool Scene::changeCamera() {
    m_active_camera = (m_active_camera+1)%_cameraconstructors.size();
    _camera.reset(_cameraconstructors[m_active_camera](QVector3D(0.0f,0.0f,10.0f), QVector3D(0.0,1.0,0.0), -90.0, 0, 45.0));

    cameraChanged(_camera.get());

    return true;
}

Camera *Scene::camera() const {
    return _camera.get();
}

const QVector<PointLight *> &Scene::pointLights() const {
    return _pointLights;
}
