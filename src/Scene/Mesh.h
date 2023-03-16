#ifndef TP_IG_M2_S2_MESH_H
#define TP_IG_M2_S2_MESH_H

#include "AbstractEntity.h"
#include "Vertex.h"
#include "Material.h"

enum MeshType {
    TRIANGLE_MESH,
    LINE_MESH,
};

struct AABB {
    QVector4D max;
    QVector4D min;
};

class ModelLoader;

class Mesh : public AbstractEntity {
    Q_OBJECT

public:
    explicit Mesh(MeshType type, QObject *parent = 0);
    ~Mesh() override;

    const QVector<Vertex> &vertices() const;
    const QVector<uint32_t> &indices() const;

    const AABB &getAabb() const;

    bool setMaterial(Material *material);

    MeshType getType() const;

    Material *material() const;

    bool isVisible() const;

    void setVisible(bool visible);

signals:
    void materialChanged(Material* material);

protected:
    friend ModelLoader;

    MeshType _type;

    QVector<Vertex> _vertices;
    QVector<uint32_t> _indices;

    Material *_material;
    AABB _aabb;

    bool _visible;
};


#endif //TP_IG_M2_S2_MESH_H
