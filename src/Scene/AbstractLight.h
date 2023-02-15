#ifndef TP_IG_M2_S2_ABSTRACTLIGHT_H
#define TP_IG_M2_S2_ABSTRACTLIGHT_H

#include <QObject>
#include <QVector3D>
#include "Mesh.h"

enum LightType {
    NONE = 0,
    POINT_LIGHT = 1,
    DIRECTIONAL_LIGHT = 2,
};

class AbstractLight : public QObject {

public:
    AbstractLight();
    explicit AbstractLight(QVector3D color);
    ~AbstractLight() override;

    LightType getType() const;

    const QVector3D &color() const;

    float intensity() const;

    Mesh *getMesh() const;
    void setMesh(Mesh *mesh);

protected:
    QVector3D _color;
    bool _enabled;
    float _intensity;
    LightType _type;
    Mesh* _marker;
};


#endif //TP_IG_M2_S2_ABSTRACTLIGHT_H
