#include <iostream>
#include "Scene.h"
#include "ModelLoader.h"

Scene::Scene() {
    setObjectName("Untilted Scene");

    _cameraconstructors.emplace_back([](QVector3D position, QVector3D up, float yaw, float pitch, float fov)->TrackballCamera*{
        std::cout << "TrackballCamera loaded" << std::endl; return new TrackballCamera(position, up, yaw, pitch, fov);
    });
    _cameraconstructors.emplace_back([](QVector3D position, QVector3D up, float yaw, float pitch, float fov)->EulerCamera*{
        std::cout << "EulerCamera loaded" << std::endl; return new EulerCamera(position, up, yaw, pitch, fov);
    });
    _camera.reset(_cameraconstructors[0](QVector3D(0.0f,0.0f,30.0f), QVector3D(0.0,1.0,0.0), -90.0, 0, 45.0));
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

const QVector<Model *> &Scene::models() const {
    return _models;
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

    ModelLoader mloader;
    Mesh *m = mloader.loadMeshFromFile("../Models/Sphere/sphere.obj");
    m->translate(pointLight->position());
    m->scale(QVector3D(0.1,0.1,0.1));
    pointLight->setMesh(m);
    m->material()->setColor(pointLight->color());
    _pointLights.push_back(pointLight);
    pointLight->setParent(this);
    lightAdded(pointLight);

    return true;
}

bool Scene::addDirectionalLight(DirectionalLight *dirLight) {
    if(_directionalLights.contains(dirLight))
        return false;

    _directionalLights.push_back(dirLight);
    dirLight->setParent(this);
    lightAdded(dirLight);

    return true;
}

bool Scene::changeEnvironment(Environment *environment) {
    _environment.reset(environment);
    environmentChanged(environment);

    return true;
}

bool Scene::changeCamera() {
    m_active_camera = (m_active_camera+1)%_cameraconstructors.size();
    _camera.reset(_cameraconstructors[m_active_camera](_camera->position(), QVector3D(0.0,1.0,0.0), _camera->getYaw(), _camera->getPitch(), 45.0));

    cameraChanged(_camera.get());

    return true;
}

Camera *Scene::camera() const {
    return _camera.get();
}

const QVector<PointLight *> &Scene::pointLights() const {
    return _pointLights;
}

const QVector<DirectionalLight *> &Scene::directionalLights() const {
    return _directionalLights;
}

void Scene::updatePosition(float dt) {

    QVector3D g = QVector3D(0, -9.8, 0);

    for(auto model_A : _models)
    {
        auto *e = (AbstractEntity*) model_A;
        if(!e->isLocked()) {
            QVector3D vn = e->getVitesse();
            QVector3D trans = vn * dt;

            bool collision = false;

            QVector<AABB> aabb_list_A;
            model_A->getAABB(aabb_list_A);
            for(auto a : aabb_list_A) {
                a.min = a.min + trans;
                a.max = a.max + trans;
            }

            for(auto model_B : _models) {
                if(model_A != model_B) {
                    QVector<AABB> aabb_list_B;
                    model_B->getAABB(aabb_list_B);
                    for(auto a : aabb_list_A) {
                        for(auto b : aabb_list_B) {
                            collision = (a.min.x() <= b.max.x() &&
                                         a.max.x() >= b.min.x() &&
                                         a.min.y() <= b.max.y() &&
                                         a.max.y() >= b.min.y() &&
                                         a.min.z() <= b.max.z() &&
                                         a.max.z() >= b.min.z());
                        }
                        if(collision)
                            break;
                    }
                }
                if(collision)
                    break;
            }

            if(!collision) {
                e->setVitesse(vn + (g / e->getMass()) * dt);
                e->translate(trans);
            }
        }
    }
}
