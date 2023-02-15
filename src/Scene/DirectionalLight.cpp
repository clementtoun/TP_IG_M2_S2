
#include "DirectionalLight.h"

DirectionalLight::DirectionalLight(QObject *parent) : AbstractLight() {
    _direction = QVector3D(0.f, 0.f, 0.f);
    _type = DIRECTIONAL_LIGHT;

    setObjectName("Untitled Point Light");
    setParent(parent);
}

DirectionalLight::DirectionalLight(QVector3D color, QVector3D direction, QObject *parent) : AbstractLight(color) {
    _direction = direction;
    _type = DIRECTIONAL_LIGHT;

    setObjectName("Untitled Point Light");
    setParent(parent);
}

const QVector3D &DirectionalLight::direction() const {
    return _direction;
}
