#include "Texture.h"
#include <QColorSpace>

Texture::Texture(Texture::TextureType type) {
    setObjectName("Untitled Texture");
    _textureType = type;
}

void Texture::setImage(const QImage &image) {
    _image = image;
    //_image.convertToColorSpace(QColorSpace::SRgb);
}

const QImage &Texture::image() const {
    return _image;
}

Texture::TextureType Texture::textureType() const {
    return _textureType;
}
