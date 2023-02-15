
#include <QOpenGLFunctions>
#include <iostream>
#include "OpenGLTexture.h"

OpenGLTexture::OpenGLTexture(Texture *texture) {
    _host = texture;
    _id = nullptr;
    glFuncs = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_3_3_Core>();
}

OpenGLTexture::~OpenGLTexture() {
    glFuncs->glDeleteTextures(1, _id);
    delete _id;
}

void OpenGLTexture::create() {
    QImage I = _host->image().convertToFormat(QImage::Format_RGBA8888).mirrored();
    _id = new unsigned int();
    glFuncs->glGenTextures(1, _id);
    glFuncs->glBindTexture(GL_TEXTURE_2D, *_id);
    glFuncs->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glFuncs->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glFuncs->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glFuncs->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFuncs->glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB_ALPHA, I.width(), I.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, I.bits());
    glFuncs->glGenerateMipmap(GL_TEXTURE_2D);
// load and generate the texture
    release();
}

bool OpenGLTexture::bind() {
    if (!_id) create();
    switch(_host->textureType()) {
        case Texture::Albedo:
        {
            glFuncs->glActiveTexture(GL_TEXTURE0 + 0);
            glFuncs->glBindTexture(GL_TEXTURE_2D, *_id);
            break;
        }
        case Texture::MetalRoughness:
        {
            glFuncs->glActiveTexture(GL_TEXTURE0 + 1);
            glFuncs->glBindTexture(GL_TEXTURE_2D, *_id);
            break;
        }
    }
    return true;
}

void OpenGLTexture::release() {
    glBindTexture( GL_TEXTURE_2D, 0);
}

void OpenGLTexture::imageChanged(const QImage &image) {
    if (_id) {
        glFuncs->glDeleteTextures(1, _id);
    }
    _host->setImage(image);
}
