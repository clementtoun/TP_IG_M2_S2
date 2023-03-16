
#include <iostream>
#include "Model.h"

Model::Model(QObject *parent) : AbstractEntity(0) {
    setObjectName("Untilted model");
    setParent(parent);
    _visible = true;
}

Model::~Model() {
    for(auto m : _childMesh)
        delete m;
    _childMesh.clear();
    for(auto m : _childModels)
        delete m;
    _childModels.clear();
}

bool Model::addChildMesh(Mesh *mesh) {
    if (!mesh || _childMesh.contains(mesh))
        return false;

    _childMesh.push_back(mesh);
    mesh->setParent(this);
    childMeshAdded(mesh);
    return true;
}

bool Model::addChildModel(Model *model) {
    if (!model || _childModels.contains(model))
        return false;

    _childModels.push_back(model);
    model->setParent(this);
    childModelAdded(model);
    return true;
}

const QVector<Mesh *> &Model::childMeshes() const {
    return _childMesh;
}

const QVector<Model *> &Model::childModels() const {
    return _childModels;
}

void Model::getAABB(QVector<AABB>& aabb_list) const {
    if(_visible) {
        AABB aabb;

        for (auto child_mesh: childMeshes()) {
            aabb = child_mesh->getAabb();

            aabb.min = child_mesh->globalModelMatrix() * aabb.min;
            aabb.max = child_mesh->globalModelMatrix() * aabb.max;
            aabb_list.push_back(aabb);
        }

        for (auto child_model: childModels()) {
            for (auto child_mesh: child_model->childMeshes()) {
                aabb = child_mesh->getAabb();
                aabb.min = child_mesh->globalModelMatrix() * aabb.min;
                aabb.max = child_mesh->globalModelMatrix() * aabb.max;
                aabb_list.push_back(aabb);
            }
        }
    }
}

void Model::setVisible(bool visible) {
    for(auto mesh : _childMesh)
        mesh->setVisible(visible);
    for(auto model : _childModels) {
        for(auto mesh : model->childMeshes()) {
            mesh->setVisible(visible);
        }
    }
    _visible = visible;
}

bool Model::isVisible() const {
    return _visible;
}
