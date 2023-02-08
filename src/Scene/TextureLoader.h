#ifndef TP_IG_M2_S2_TEXTURELOADER_H
#define TP_IG_M2_S2_TEXTURELOADER_H

#include "Texture.h"
#include <QWeakPointer>

class TextureLoader {
public:
    TextureLoader() = default;

    QSharedPointer<Texture> loadFromFile(Texture::TextureType textureType, QString filePath);

private:
    static QHash<QString, QWeakPointer<Texture>> _cache;
};


#endif //TP_IG_M2_S2_TEXTURELOADER_H
