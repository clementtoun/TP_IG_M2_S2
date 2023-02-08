#include "OpenGLUniformBufferObject.h"

QOpenGLFunctions_3_3_Core* OpenGLUniformBufferObject::glFuncs = 0;

OpenGLUniformBufferObject::OpenGLUniformBufferObject() {
    _id = 0;
    glFuncs = nullptr;
}

OpenGLUniformBufferObject::~OpenGLUniformBufferObject() {
    if (_id) destroy();
}

GLuint OpenGLUniformBufferObject::bufferId() const {
    return _id;
}

bool OpenGLUniformBufferObject::create() {
    if (_id) destroy();
    if (!glFuncs)
        glFuncs = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_3_3_Core>();
    glFuncs->glGenBuffers(1, &_id);
    return _id > 0;
}

void OpenGLUniformBufferObject::allocate(int indx, const void * data, int count) const {
    if (_id == 0) return;
    if (!glFuncs) return;
    glFuncs->glBufferData(GL_UNIFORM_BUFFER, count, data, GL_STATIC_DRAW);
    glFuncs->glBindBufferRange(GL_UNIFORM_BUFFER, indx, _id, 0, count);
}

void OpenGLUniformBufferObject::destroy() {
    if (_id && glFuncs) {
        glFuncs->glDeleteBuffers(1, &_id);
        _id = 0;
    }
}

void OpenGLUniformBufferObject::bind() const {
    if (_id && glFuncs)
        glFuncs->glBindBuffer(GL_UNIFORM_BUFFER, _id);
}

void OpenGLUniformBufferObject::write(int offset, const void * data, int count) const {
    if (_id && glFuncs)
        glFuncs->glBufferSubData(GL_UNIFORM_BUFFER, offset, count, data);
}

void OpenGLUniformBufferObject::release() {
    if (glFuncs)
        glFuncs->glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void OpenGLUniformBufferObject::bindUniformBlock(QOpenGLShaderProgram* shader) {
    if (!glFuncs)
        glFuncs = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_3_3_Core>();
    GLuint indx = glFuncs->glGetUniformBlockIndex(shader->programId(), "CameraInfo");
    glFuncs->glUniformBlockBinding(shader->programId(), indx, CAMERA_INFO_BINDING_POINT);
    indx = glFuncs->glGetUniformBlockIndex(shader->programId(), "ModelInfo");
    glFuncs->glUniformBlockBinding(shader->programId(), indx, MODEL_INFO_BINDING_POINT);
    indx = glFuncs->glGetUniformBlockIndex(shader->programId(), "MaterialInfo");
    glFuncs->glUniformBlockBinding(shader->programId(), indx, MATERIAL_INFO_BINDING_POINT);
    indx = glFuncs->glGetUniformBlockIndex(shader->programId(), "LightInfo");
    glFuncs->glUniformBlockBinding(shader->programId(), indx, LIGHT_INFO_BINDING_POINT);
}