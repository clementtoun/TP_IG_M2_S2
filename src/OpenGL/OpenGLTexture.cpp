
#include <QOpenGLFunctions>
#include <iostream>
#include "OpenGLTexture.h"

OpenGLTexture::OpenGLTexture(Texture *texture) {
    _host = texture;
    _openGLTexture = 0;
}

OpenGLTexture::~OpenGLTexture() {
    _openGLTexture->destroy();
    delete _openGLTexture;
}

void OpenGLTexture::create() {
    _openGLTexture = new QOpenGLTexture(_host->image().mirrored());
    _openGLTexture->setMinificationFilter(QOpenGLTexture::Nearest);
    _openGLTexture->setMagnificationFilter(QOpenGLTexture::Linear);
    _openGLTexture->setWrapMode(QOpenGLTexture::Repeat);
}

bool OpenGLTexture::bind() {
    if (!_openGLTexture) create();
    QOpenGLFunctions * glFuncs = QOpenGLContext::currentContext()->functions();
    switch(_host->textureType()) {
        case Texture::Albedo:
        {
            glFuncs->glActiveTexture(GL_TEXTURE0 + 0);
            glFuncs->glBindTexture(GL_TEXTURE_2D, _openGLTexture->textureId());
            break;
        }
        case Texture::MetalRoughness:
        {
            glFuncs->glActiveTexture(GL_TEXTURE0 + 1);
            glFuncs->glBindTexture(GL_TEXTURE_2D, _openGLTexture->textureId());
            break;
        }
    }
    return true;
}

void OpenGLTexture::release() {
    _openGLTexture->release();
}

void OpenGLTexture::imageChanged(const QImage &image) {
    if (_openGLTexture) {
        delete _openGLTexture;
        _openGLTexture = new QOpenGLTexture(image);
        _openGLTexture->setMinificationFilter(QOpenGLTexture::Nearest);
        _openGLTexture->setMagnificationFilter(QOpenGLTexture::Linear);
        _openGLTexture->setWrapMode(QOpenGLTexture::Repeat);
    }
}
