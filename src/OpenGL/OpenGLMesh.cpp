#include <QMatrix4x4>
#include <iostream>
#include "OpenGLMesh.h"

struct ShaderModelInfo {
    float modelMat[16];   // 64          // 0
    float normalMat[16];  // 64          // 64
};

static ShaderModelInfo shaderModelInfo;

OpenGLUniformBufferObject *OpenGLMesh::_modelInfo = 0;

OpenGLMesh::OpenGLMesh(Mesh *mesh, QObject *parent) : QObject(nullptr) {
    _host = mesh;
    _type = _host->getType();
    _vao = nullptr;
    _vbo = nullptr;
    _ebo = nullptr;

    if (_host->material())
        _openGLMaterial = new OpenGLMaterial(_host->material());
    else
        _openGLMaterial = 0;

    connect(_host, SIGNAL(materialChanged(Material*)), this, SLOT(materialChanged(Material*)));

    setParent(parent);
}

OpenGLMesh::~OpenGLMesh() {
    this->destroy();
    delete _openGLMaterial;
}

Mesh *OpenGLMesh::host() const {
    return _host;
}

void OpenGLMesh::create() {
    this->destroy();
    _vao = new QOpenGLVertexArrayObject;

    _vao->create();
    _vao->bind();
    _vbo = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
    _vbo->create();
    _vbo->bind();
    if (_host->vertices().size())
        _vbo->allocate(&_host->vertices()[0], int(sizeof(Vertex) * _host->vertices().size()));
    _ebo = new QOpenGLBuffer(QOpenGLBuffer::IndexBuffer);
    _ebo->create();
    _ebo->bind();
    if (_host->indices().size())
        _ebo->allocate(&_host->indices()[0], int(sizeof(uint32_t) * _host->indices().size()));

    glFuncs = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_4_5_Core>();
    glFuncs->glEnableVertexAttribArray(0);
    glFuncs->glEnableVertexAttribArray(1);
    glFuncs->glEnableVertexAttribArray(2);
    glFuncs->glEnableVertexAttribArray(3);
    glFuncs->glEnableVertexAttribArray(4);
    glFuncs->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, position));
    glFuncs->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, normal));
    glFuncs->glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, texCoords));
    glFuncs->glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, tangent));
    glFuncs->glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, bitangent));
}

void OpenGLMesh::commit() {
    QMatrix4x4 modelMat = _host->globalModelMatrix();

    memcpy(shaderModelInfo.modelMat, modelMat.constData(), 64);
    memcpy(shaderModelInfo.normalMat, QMatrix4x4(modelMat.normalMatrix()).constData(), 64);

    if (_modelInfo == 0) {
        _modelInfo = new OpenGLUniformBufferObject;
        _modelInfo->create();
        _modelInfo->bind();
        _modelInfo->allocate(MODEL_INFO_BINDING_POINT, NULL, sizeof(ShaderModelInfo));
        _modelInfo->release();
    }

    _modelInfo->bind();
    _modelInfo->write(0, &shaderModelInfo, sizeof(ShaderModelInfo));
    _modelInfo->release();
}

void OpenGLMesh::render() {
    if (_vao == 0 || _vbo == 0 || _ebo == 0) create();
    commit();

    if (_openGLMaterial)
        _openGLMaterial->bind();

    _vao->bind();
    switch (_type) {
        case TRIANGLE_MESH:
        {
            glFuncs->glDrawElements(GL_TRIANGLES, (GLsizei) _host->indices().size(), GL_UNSIGNED_INT,0);
            break;
        }
        case LINE_MESH:
        {
            glFuncs->glDrawElements(GL_LINE, (GLsizei) _host->indices().size(), GL_UNSIGNED_INT,0);
            break;
        }
    }
    _vao->release();

    if (_openGLMaterial)
        _openGLMaterial->release();
}

void OpenGLMesh::destroy() {
    delete _vao;
    delete _vbo;
    delete _ebo;
    _vao = nullptr;
    _vbo = nullptr;
    _ebo = nullptr;
}

void OpenGLMesh::materialChanged(Material *material) {
    if (material == 0)
        _openGLMaterial = 0;
    else {
        _openGLMaterial = new OpenGLMaterial(material);
    }
}


