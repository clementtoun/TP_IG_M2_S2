
#include <QOpenGLShaderProgram>
#include "OpenGLEnvironment.h"

OpenGLEnvironment::OpenGLEnvironment(Environment *environment, QObject *parent) : QObject(0) {
    _host = environment;
    _vao = nullptr;
    _vbo = nullptr;

    setParent(parent);
}

void OpenGLEnvironment::create() {
    this->destroy();

    _vao = new QOpenGLVertexArrayObject;
    _vao->create();
    _vao->bind();

    _vbo = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
    _vbo->create();
    _vbo->bind();
    _vbo->allocate(_cubeMapVertices, sizeof(_cubeMapVertices));

    glFuncs = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_3_3_Core>();

    glFuncs->glEnableVertexAttribArray(0);
    glFuncs->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    QOpenGLShaderProgram pano_to_cube_shader;
    pano_to_cube_shader.addCacheableShaderFromSourceFile(QOpenGLShader::Vertex,"../src/Shader/pano_to_cube_Vert.shader");
    pano_to_cube_shader.addCacheableShaderFromSourceFile(QOpenGLShader::Fragment,"../src/Shader/pano_to_cube_Frag.shader");
    pano_to_cube_shader.link();

    QImage image = _host->getEnvironmentMap()->image();
    QOpenGLTexture hdri(image);
    hdri.setWrapMode(QOpenGLTexture::ClampToEdge);
    hdri.setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
    hdri.setMagnificationFilter(QOpenGLTexture::LinearMipMapLinear);

    _environmentTexture = new QOpenGLTexture(QOpenGLTexture::Target::TargetCubeMap);
    _environmentTexture->create();
    _environmentTexture->setSize(2500, 2500, image.depth());
    _environmentTexture->setFormat(QOpenGLTexture::RGBA8_UNorm);
    _environmentTexture->allocateStorage();
    unsigned int fbo;
    glFuncs->glGenFramebuffers(1, &fbo);
    glFuncs->glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    for (unsigned int i = 0; i < 6; i++) {
        glFuncs->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                                  GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, _environmentTexture->textureId(), 0);
        _environmentTexture->bind();
        glFuncs->glViewport(0, 0, _environmentTexture->width(), _environmentTexture->height());
        glFuncs->glClearColor(0.5f, 0.5f, 0.5f, 0.f);
        glFuncs->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        pano_to_cube_shader.bind();
        glFuncs->glActiveTexture(GL_TEXTURE0);
        hdri.bind();
        pano_to_cube_shader.setUniformValue("u_panorama", 0);
        pano_to_cube_shader.setUniformValue("u_currentFace", i);

        glFuncs->glDrawArrays(GL_TRIANGLES, 0, 3);
    }

    glFuncs->glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glFuncs->glDeleteFramebuffers(1, &fbo);
}

bool OpenGLEnvironment::bind() {
    glFuncs->glActiveTexture(GL_TEXTURE0);
    glFuncs->glBindTexture(GL_TEXTURE_2D, _environmentTexture->textureId());

    return true;
}

void OpenGLEnvironment::release() {
    _environmentTexture->release();
}

void OpenGLEnvironment::render() {
    if (_vao == 0 || _vbo == 0) create();

    glFuncs->glDepthFunc(GL_LEQUAL);

    bind();

    _environmentTexture->bind();
    _vao->bind();
    glFuncs->glDrawArrays(GL_TRIANGLES, 0, 36);
    _vao->release();

    release();

    glFuncs->glDepthFunc(GL_LESS);
}

void OpenGLEnvironment::destroy() {
    delete _vao;
    delete _vbo;
    _vao = nullptr;
    _vbo = nullptr;
}

OpenGLEnvironment::~OpenGLEnvironment() {
    _environmentTexture->destroy();
    delete _environmentTexture;
    this->destroy();
}
