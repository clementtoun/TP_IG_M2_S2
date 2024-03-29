#ifndef TP_IG_M2_S2_MATERIAL_H
#define TP_IG_M2_S2_MATERIAL_H

#include <QVector3D>
#include <QSharedPointer>
#include "Texture.h"

class Material : public QObject {
    Q_OBJECT

public:
    explicit Material(QObject * parent = 0);
    Material(QVector3D color, float metallic, float roughness, QObject * parent = 0);

    const QSharedPointer<Texture> &getColorTexture() const;
    const QSharedPointer<Texture> &getMetallicRoughnessTexture() const;
    const QVector3D &getColor() const;
    float getMetallic() const;
    float getRoughness() const;

    const QSharedPointer<Texture> &getNormalTexture() const;
    const QSharedPointer<Texture> &getAoTexture() const;

    const QSharedPointer<Texture> &getEmissiveTexture() const;

    void setColor(const QVector3D &color);
    void setMetallic(float metallic);
    void setRoughness(float roughness);
    void setColorTexture(const QSharedPointer<Texture> &colorTexture);
    void setMetallicRoughnessTexture(const QSharedPointer<Texture> &metallicRoughnessTexture);
    void setNormalTexture(const QSharedPointer<Texture> &normalTexture);
    void setAoTexture(const QSharedPointer<Texture> &aoTexture);
    void setEmissiveTexture(const QSharedPointer<Texture> &emissiveTexture);

protected:
    QVector3D _color;
    float _metallic, _roughness;
    QSharedPointer<Texture> _colorTexture, _metallicRoughnessTexture, _normalTexture, _aoTexture, _emissiveTexture;
};


#endif //TP_IG_M2_S2_MATERIAL_H
