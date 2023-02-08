
#include "OpenGLRenderer.h"

OpenGLRenderer::OpenGLRenderer(QObject *parent) : QObject(nullptr) {
    _basicShader = nullptr;
    setParent(parent);
}

bool OpenGLRenderer::reloadShaders() {
    delete _basicShader;
    _basicShader = nullptr;

    _basicShader = new QOpenGLShaderProgram();
    _basicShader->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, "../src/Shader/Gltf_Vert.shader");
    _basicShader->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment,"../src/Shader/Gltf_Frag.shader");
    _basicShader->link();

    OpenGLUniformBufferObject::bindUniformBlock(_basicShader);

    _basicShader->bind();
    _basicShader->setUniformValue("colorTexture", 0);
    _basicShader->setUniformValue("metallicRoughnessTexture", 1);

    _cubeMapShader = new QOpenGLShaderProgram();
    _cubeMapShader->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, "../src/Shader/cubeMap_Vert.shader");
    _cubeMapShader->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, "../src/Shader/cubeMap_Frag.shader");
    _cubeMapShader->link();

    _cubeMapShader->bind();
    _cubeMapShader->setUniformValue("skybox", 0);

    return _basicShader;
}

void OpenGLRenderer::render(OpenGLScene *openGLScene) {

    if (_basicShader) {
        _basicShader->bind();
        openGLScene->renderModels();
    }

    if(_cubeMapShader) {
        _cubeMapShader->bind();
        openGLScene->renderEnvironment();
    }
}

OpenGLRenderer::~OpenGLRenderer() {
    delete _basicShader;
    delete _cubeMapShader;
}
