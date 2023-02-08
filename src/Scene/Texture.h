#ifndef TP_IG_M2_S2_TEXTURE_H
#define TP_IG_M2_S2_TEXTURE_H

#include <QObject>
#include <QImage>

class Texture : public QObject {
    Q_OBJECT

public:
    enum TextureType {
        Albedo = 0,
        MetalRoughness = 1,
    };

    explicit Texture(TextureType type);
    ~Texture() override = default;

    void setImage(const QImage &image);
    const QImage &image() const;

    TextureType textureType() const;

private:
    TextureType _textureType;
    QImage _image;
};


#endif //TP_IG_M2_S2_TEXTURE_H
