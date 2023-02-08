
#include "PointLight.h"

PointLight::PointLight(QObject *parent) : AbstractLight() {
    _position = QVector3D(0.f, 0.f, 0.f);
    _attenuationQuadratic = 0.0007f;
    _attenuationLinear = 0.014f;
    _attenuationConstant = 1.0f;

    setObjectName("Untitled Point Light");
    setParent(parent);
}

PointLight::PointLight(QVector3D color, QVector3D position, QObject *parent) : AbstractLight(color) {
    _position = position;
    _attenuationQuadratic = 0.000007f;
    _attenuationLinear = 0.0014f;
    _attenuationConstant = 1.0f;

    setObjectName("Untitled Point Light");
    setParent(parent);
}

const QVector3D &PointLight::position() const {
    return _position;
}

float PointLight::attenuationQuadratic() const {
    return _attenuationQuadratic;
}

float PointLight::attenuationLinear() const {
    return _attenuationLinear;
}

float PointLight::attenuationConstant() const {
    return _attenuationConstant;
}

PointLight::~PointLight() = default;
