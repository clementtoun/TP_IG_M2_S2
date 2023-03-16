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

    void getAABB(QVector<AABB>& aabb_list) const;

    const QVector<Mesh*> &childMeshes() const;
    const QVector<Model*> &childModels() const;

    void setVisible(bool visible);

    bool isVisible() const;

signals:
    void childMeshAdded(Mesh* mesh);
    void childModelAdded(Model* model);

private:
    QVector<Mesh*> _childMesh;
    QVector<Model*> _childModels;

    bool _visible;
};


#endif //TP_IG_M2_S2_MODEL_H
