#include <QMatrix4x4>
#include <iostream>
#include "AbstractEntity.h"

AbstractEntity::AbstractEntity(QObject *parent) : QObject(0) {
    _locked = true;
    _vitesse = QVector3D(0.0, 0.0, 0.0);
    _mass = 1.0;
    _position = QVector3D(0.0,0.0,0.0);
    _rotation = QVector3D(0.0,0.0,0.0);
    _scale = QVector3D(1.0,1.0,1.0);
    setObjectName("Untitled Entity");
    setParent(parent);
}

AbstractEntity::~AbstractEntity() = default;

void AbstractEntity::translate(QVector3D delta) {
    _position += delta;
}

void AbstractEntity::rotate(QQuaternion rotation) {
    _rotation = (QQuaternion::fromEulerAngles(_rotation) * rotation).toEulerAngles();
}

void AbstractEntity::rotate(QVector3D rotation) {
    _rotation = (QQuaternion::fromEulerAngles(_rotation) * QQuaternion::fromEulerAngles(rotation)).toEulerAngles();
}

void AbstractEntity::scale(QVector3D scaling) {
    _scale *= scaling;
}

QMatrix4x4 AbstractEntity::globalModelMatrix() const {
    if (AbstractEntity* par = qobject_cast<AbstractEntity*>(parent())) {
        return par->globalModelMatrix() * localModelMatrix();
    }
    else
        return localModelMatrix();
}

QMatrix4x4 AbstractEntity::localModelMatrix() const {
    QMatrix4x4 model;

    model.translate(_position);
    model.rotate(QQuaternion::fromEulerAngles(_rotation));
    model.scale(_scale);

    return model;
}

void AbstractEntity::setLocalModel(const QMatrix4x4 &model) {
    _position = model.column(3).toVector3D();
    _scale = QVector3D(model.column(0).length(), model.column(1).length(), model.column(2).length());
    QMatrix4x4 R = QMatrix4x4();
    R.setColumn(0, model.column(0)/_scale.x());
    R.setColumn(1, model.column(1)/_scale.y());
    R.setColumn(2, model.column(2)/_scale.z());
    _rotation = QQuaternion::fromRotationMatrix(R.normalMatrix()).toEulerAngles();
}

bool AbstractEntity::isLocked() const {
    return _locked;
}

void AbstractEntity::setLocked(bool locked) {
    _locked = locked;
}

float AbstractEntity::getMass() const {
    return _mass;
}

void AbstractEntity::setMass(float mass) {
    _mass = mass;
}

const QVector3D &AbstractEntity::getVitesse() const {
    return _vitesse;
}

void AbstractEntity::setVitesse(const QVector3D &vitesse) {
    _vitesse = vitesse;
}

const QVector3D &AbstractEntity::getPosition() const {
    return _position;
}

void AbstractEntity::setPosition(const QVector3D &position) {
    _position = position;
}

const QVector3D &AbstractEntity::getRotation() const {
    return _rotation;
}

void AbstractEntity::setRotation(const QVector3D &rotation) {
    _rotation = rotation;
}

const QVector3D &AbstractEntity::getTranslation() const {
    return _translation;
}

void AbstractEntity::setTranslation(const QVector3D &translation) {
    _translation = translation;
}
