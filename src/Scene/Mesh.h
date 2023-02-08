#ifndef TP_IG_M2_S2_MESH_H
#define TP_IG_M2_S2_MESH_H

#include "AbstractEntity.h"
#include "Vertex.h"
#include "Material.h"

class ModelLoader;

class Mesh : public AbstractEntity {
    Q_OBJECT

public:
    explicit Mesh(QObject *parent = 0);
    ~Mesh() override;

    const QVector<Vertex> &vertices() const;
    const QVector<uint32_t> &indices() const;

    bool setMaterial(Material *material);
    Material *material() const;

signals:
    void materialChanged(Material* material);

protected:
    friend ModelLoader;
    QVector<Vertex> _vertices;
    QVector<uint32_t> _indices;
    Material *_material;
};


#endif //TP_IG_M2_S2_MESH_H
