#include <QMatrix4x4>
#include <iostream>
#include "AbstractEntity.h"

AbstractEntity::AbstractEntity(QObject *parent) : QObject(0) {
    _model.setToIdentity();
    setObjectName("Untitled Entity");
    setParent(parent);
}

AbstractEntity::~AbstractEntity() = default;

void AbstractEntity::translate(QVector3D delta) {
    _model.translate(delta);
}

void AbstractEntity::rotate(QQuaternion rotation) {
    _model.rotate(rotation);
}

void AbstractEntity::rotate(QVector3D rotation) {
    _model.rotate(QQuaternion::fromEulerAngles(rotation));
}

void AbstractEntity::scale(QVector3D scaling) {
    _model.scale(scaling);
}

QMatrix4x4 AbstractEntity::globalModelMatrix() const {
    if (AbstractEntity* par = qobject_cast<AbstractEntity*>(parent())) {
        return par->globalModelMatrix() * localModelMatrix();
    }
    else
        return localModelMatrix();
}

QMatrix4x4 AbstractEntity::localModelMatrix() const {
    return _model;
}

void AbstractEntity::setLocalModel(const QMatrix4x4 &model) {
    _model = model;
}
