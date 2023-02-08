
#ifndef TP_IG_M2_S2_POINTLIGHT_H
#define TP_IG_M2_S2_POINTLIGHT_H

#include <QVector3D>
#include "Mesh.h"
#include "AbstractLight.h"

class PointLight : public AbstractLight {
    Q_OBJECT

public:
    explicit PointLight(QObject* parent = 0);
    PointLight(QVector3D color, QVector3D position, QObject* parent = 0);
    ~PointLight();

    const QVector3D &position() const;
    float attenuationQuadratic() const;
    float attenuationLinear() const;
    float attenuationConstant() const;

private:
    QVector3D _position;
    float _attenuationQuadratic, _attenuationLinear, _attenuationConstant;
    Mesh* _marker;
};


#endif //TP_IG_M2_S2_POINTLIGHT_H
