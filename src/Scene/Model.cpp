
#include "Model.h"

Model::Model(QObject *parent) : AbstractEntity(0) {
    setObjectName("Untilted model");
    setParent(parent);
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
    return true;
}

const QVector<Mesh *> &Model::childMeshes() const {
    return _childMesh;
}

const QVector<Model *> &Model::childModels() const {
    return _childModels;
}
