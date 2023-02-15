#ifndef TP_IG_M2_S2_DIRECTIONALLIGHT_H
#define TP_IG_M2_S2_DIRECTIONALLIGHT_H

#include "AbstractLight.h"

class DirectionalLight : public AbstractLight {
    Q_OBJECT

public:
    explicit DirectionalLight(QObject* parent = 0);
    DirectionalLight(QVector3D color, QVector3D direction, QObject* parent = 0);
    ~DirectionalLight() override = default;

    const QVector3D &direction() const;

private:
    QVector3D _direction;
};


#endif //TP_IG_M2_S2_DIRECTIONALLIGHT_H
