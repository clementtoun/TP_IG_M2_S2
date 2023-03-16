#include "Mesh.h"

Mesh::Mesh(MeshType type, QObject *parent) : _type(type), AbstractEntity(nullptr) {
    setObjectName("Untitled model");
    setParent(parent);
    _material = nullptr;
    _visible = true;
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

const AABB &Mesh::getAabb() const {
    return _aabb;
}

MeshType Mesh::getType() const {
    return _type;
}

bool Mesh::isVisible() const {
    return _visible;
}

void Mesh::setVisible(bool visible) {
    _visible = visible;
}

