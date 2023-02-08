#include "Environment.h"

Environment::Environment(QObject * parent) : QObject(0) {
    setObjectName("Untitled Environment");
    setParent(parent);
}

const QSharedPointer<Texture> &Environment::getEnvironmentMap() const {
    return _environmentMap;
}

void Environment::setEnvironmentMap(const QSharedPointer<Texture> &environmentMap) {
    _environmentMap = environmentMap;
}
