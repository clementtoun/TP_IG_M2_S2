#include <iostream>
#include "OpenGLScene.h"

struct ShaderCameraInfo { // struct size: 144
    //                         // base align  // aligned offset
    float projMat[16];         // 64          // 0
    float viewMat[16];         // 64          // 64
    QVector4D cameraPos;       // 16          // 128
} shaderCameraInfo;

struct ShaderPointLight { // struct size: 48
    //                           // base align  // aligned offset
    QVector4D color;             // 16          // 0
    QVector4D pos;               // 16          // 16
    QVector4D attenuation;       // 16          // 32
};

struct ShaderDirectionalLight {
    QVector4D color;
    QVector4D dir;
};

struct ShaderlightInfo { // struct size: 1424
    //                                          // base align  // aligned offset
    //int ambientLightNum;                        // 4           // 0     // 4           // 4     // 4           // 8
    //int spotLightNum;                           // 4           // 12
    //ShaderAmbientLight ambientLight[8];         // 16          // 16
    ShaderDirectionalLight directionalLight; // 32          // 144
    ShaderPointLight pointLight;             // 48          // 400
    int directionalLightNum;
    int pointLightNum;
    //ShaderSpotLight spotLight[8];               // 80          // 784
};

static ShaderlightInfo shaderlightInfo;
OpenGLUniformBufferObject *OpenGLScene::_lightInfo = 0;

OpenGLUniformBufferObject *OpenGLScene::_cameraInfo = 0;

OpenGLScene::OpenGLScene(Scene *scene) {
    _host = scene;
    connect(_host, SIGNAL(modelAdded(Model*)), this, SLOT(modelAdded(Model*)));
    connect(_host, SIGNAL(lightAdded(AbstractLight *)), this, SLOT(lightAdded(AbstractLight *)));
    connect(_host, SIGNAL(environmentChanged(Environment *)), this, SLOT(environmentChanged(Environment *)));
}

Scene *OpenGLScene::host() const {
    return _host;
}

void OpenGLScene::renderModels() {
    if(_environment.get())
        _environment->bind();
    for (auto mesh: _normalMeshes) {
        if(mesh->host()->isVisible())
            mesh->render();
    }
}

void OpenGLScene::renderLights() {
    for (auto mesh : _lightMeshes) {
        mesh->render();
    }
}

void OpenGLScene::renderEnvironment() {
    if(_environment.get())
        _environment->render();
}

void OpenGLScene::modelAdded(Model *model) {
    connect(model, SIGNAL(childMeshAdded(Mesh*)), this, SLOT(meshAdded(Mesh*)));
    for (int i = 0; i < model->childMeshes().size(); i++)
        meshAdded(model->childMeshes()[i]);
    for (int i = 0; i < model->childModels().size(); i++)
        modelAdded(model->childModels()[i]);
}

void OpenGLScene::meshAdded(Mesh *mesh) {
    _normalMeshes.push_back(new OpenGLMesh(mesh, this));
}

void OpenGLScene::lightAdded(AbstractLight *light) {
    if(light->getType() == POINT_LIGHT)
        _lightMeshes.push_back(new OpenGLMesh(light->getMesh(), this));
}

void OpenGLScene::environmentChanged(Environment *environment) {
    _environment.reset(new OpenGLEnvironment(environment, this));
}

void OpenGLScene::commitCameraInfo() {
    if (!_host->camera()) return;
    memcpy(shaderCameraInfo.projMat, _host->camera()->projectionmatrix().constData(), 64);
    memcpy(shaderCameraInfo.viewMat, _host->camera()->viewmatrix().constData(), 64);
    shaderCameraInfo.cameraPos = _host->camera()->position();

    if (_cameraInfo == 0) {
        _cameraInfo = new OpenGLUniformBufferObject;
        _cameraInfo->create();
        _cameraInfo->bind();
        _cameraInfo->allocate(CAMERA_INFO_BINDING_POINT, NULL, sizeof(ShaderCameraInfo));
        _cameraInfo->release();
    }

    _cameraInfo->bind();
    _cameraInfo->write(0, &shaderCameraInfo, sizeof(ShaderCameraInfo));
    _cameraInfo->release();
}

void OpenGLScene::commitLightInfo(AbstractLight *light) {
    int pointLightNum = 0;
    if(light->getType() == POINT_LIGHT) {
        auto *l = (PointLight *) light;
        shaderlightInfo.pointLight.color = l->color() * l->intensity();
        shaderlightInfo.pointLight.pos = l->position();
        shaderlightInfo.pointLight.attenuation[0] = 1.f;
        shaderlightInfo.pointLight.attenuation[1] = l->attenuationQuadratic();
        shaderlightInfo.pointLight.attenuation[2] = l->attenuationLinear();
        shaderlightInfo.pointLight.attenuation[3] = l->attenuationConstant();
        pointLightNum++;
    }
    int directionalLightNum = 0;
    if(light->getType() == DIRECTIONAL_LIGHT) {
        auto *l = (DirectionalLight *) light;
        shaderlightInfo.directionalLight.color = l->color() * l->intensity();
        shaderlightInfo.directionalLight.dir = l->direction();
        directionalLightNum++;
    }

    shaderlightInfo.pointLightNum = pointLightNum;
    shaderlightInfo.directionalLightNum = directionalLightNum;

    if (_lightInfo == 0) {
        _lightInfo = new OpenGLUniformBufferObject;
        _lightInfo->create();
        _lightInfo->bind();
        _lightInfo->allocate(LIGHT_INFO_BINDING_POINT, NULL, sizeof(ShaderlightInfo));
        _lightInfo->release();
    }
    _lightInfo->bind();
    _lightInfo->write(0, &shaderlightInfo, sizeof(ShaderlightInfo));
    _lightInfo->release();
}

OpenGLScene::~OpenGLScene() {
    for(auto m : _normalMeshes)
        delete m;
    _normalMeshes.clear();
}

bool OpenGLScene::hasEnvironment() {
    return _environment.get() != nullptr;
}

