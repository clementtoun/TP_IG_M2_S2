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

struct ShaderlightInfo { // struct size: 1424
    //                                          // base align  // aligned offset
    //int ambientLightNum;                        // 4           // 0
    //int directionalLightNum;                    // 4           // 4     // 4           // 8
    //int spotLightNum;                           // 4           // 12
    //ShaderAmbientLight ambientLight[8];         // 16          // 16
    //ShaderDirectionalLight directionalLight[8]; // 32          // 144
    ShaderPointLight pointLight[8];             // 48          // 400
    //ShaderSpotLight spotLight[8];               // 80          // 784
    int pointLightNum;
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
    for (auto mesh : _normalMeshes) {
        mesh->render();
    }
}

void OpenGLScene::renderLights() {

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

void OpenGLScene::commitLightInfo() {
    int pointLightNum = 0;
    for (int i = 0; i < _host->pointLights().size(); i++) {
        shaderlightInfo.pointLight[pointLightNum].color = _host->pointLights()[i]->color() * _host->pointLights()[i]->intensity();
        shaderlightInfo.pointLight[pointLightNum].pos = _host->pointLights()[i]->position();
        shaderlightInfo.pointLight[pointLightNum].attenuation[0] = 1.f;
        shaderlightInfo.pointLight[pointLightNum].attenuation[1] = _host->pointLights()[i]->attenuationQuadratic();
        shaderlightInfo.pointLight[pointLightNum].attenuation[2] = _host->pointLights()[i]->attenuationLinear();
        shaderlightInfo.pointLight[pointLightNum].attenuation[3] = _host->pointLights()[i]->attenuationConstant();
        pointLightNum++;
    }

    shaderlightInfo.pointLightNum = pointLightNum;

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

