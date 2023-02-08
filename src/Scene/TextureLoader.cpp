#include <QImageReader>
#include "TextureLoader.h"

QHash<QString, QWeakPointer<Texture>> TextureLoader::_cache;


QSharedPointer<Texture> TextureLoader::loadFromFile(Texture::TextureType textureType, QString filePath) {
    if (_cache[filePath].isNull()) {
        QSharedPointer<Texture> texture(new Texture(textureType));
        QImageReader reader(filePath);
        texture->setObjectName(filePath);
        texture->setImage(reader.read());

        if (texture->image().isNull()) {
            return {};
        }

        _cache[filePath] = texture;
        return texture;
    }

    return _cache[filePath];
}
