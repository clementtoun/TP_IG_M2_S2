#ifndef TP_IG_M2_S2_MODEL_H
#define TP_IG_M2_S2_MODEL_H

#include "AbstractEntity.h"
#include "Mesh.h"

class Model : public AbstractEntity {
    Q_OBJECT

public:
    explicit Model(QObject *parent = 0);
    ~Model() override;

    bool addChildMesh(Mesh* mesh);
    bool addChildModel(Model* model);

    const QVector<Mesh*> &childMeshes() const;
    const QVector<Model*> &childModels() const;

signals:
    void childMeshAdded(Mesh* mesh);

private:
    QVector<Mesh*> _childMesh;
    QVector<Model*> _childModels;
};


#endif //TP_IG_M2_S2_MODEL_H
