#include "AbstractLight.h"

AbstractLight::AbstractLight() {
    _color = QVector3D(1.f, 1.f, 1.f);
    _intensity = 1.f;
    setObjectName("Untitled Light");
}

AbstractLight::AbstractLight(QVector3D color) {
    _color = color;
    _intensity = 1.f;
    setObjectName("Untitled Light");
}

AbstractLight::~AbstractLight() {}

const QVector3D &AbstractLight::color() const {
    return _color;
}

float AbstractLight::intensity() const {
    return _intensity;
}

LightType AbstractLight::getType() const {
    return _type;
}

Mesh *AbstractLight::getMesh() const {
    return _marker;
}

void AbstractLight::setMesh(Mesh *mesh) {
    _marker = mesh;
}
