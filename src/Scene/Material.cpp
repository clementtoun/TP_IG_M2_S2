
#include "Material.h"

Material::Material(QObject *parent) : QObject(parent) {
    _color = QVector3D(1.0f, 1.0f, 1.0f);
    _metallic = 0.5f;
    _roughness = 0.5f;
    setObjectName("Untitled Material");
    setParent(parent);
}

Material::Material(QVector3D color, float metallic, float roughness, QObject *parent) {
    _color = color;
    _metallic = metallic;
    _roughness = roughness;
    setObjectName("Untitled Material");
    setParent(parent);
}

const QSharedPointer<Texture> &Material::getColorTexture() const {
    return _colorTexture;
}

const QSharedPointer<Texture> &Material::getMetallicRoughnessTexture() const {
    return _metallicRoughnessTexture;
}

const QVector3D &Material::getColor() const {
    return _color;
}

float Material::getMetallic() const {
    return _metallic;
}

float Material::getRoughness() const {
    return _roughness;
}

void Material::setColor(const QVector3D &color) {
    _color = color;
}

void Material::setMetallic(float metallic) {
    _metallic = metallic;
}

void Material::setRoughness(float roughness) {
    _roughness = roughness;
}

void Material::setColorTexture(const QSharedPointer<Texture> &colorTexture) {
    _colorTexture = colorTexture;
}

void Material::setMetallicRoughnessTexture(const QSharedPointer<Texture> &metallicRoughnessTexture) {
    _metallicRoughnessTexture = metallicRoughnessTexture;
}
