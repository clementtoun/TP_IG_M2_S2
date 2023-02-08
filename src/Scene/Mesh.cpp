#include "Mesh.h"

Mesh::Mesh(QObject *parent) : AbstractEntity(nullptr) {
    setObjectName("Untitled model");
    setParent(parent);
    _material = nullptr;
}

Mesh::~Mesh() {
    delete _material;
}

const QVector<Vertex> &Mesh::vertices() const {
    return _vertices;
}

const QVector<uint32_t> &Mesh::indices() const {
    return _indices;
}

bool Mesh::setMaterial(Material *material) {
    if (_material == material) return false;

    if (_material != nullptr) {
        Material* tmp = _material;
        _material = 0;
        delete tmp;
    }

    if (material) {
        _material = material;
        _material->setParent(this);
    }

    materialChanged(material);
    return true;
}

Material *Mesh::material() const {
    return _material;
}

