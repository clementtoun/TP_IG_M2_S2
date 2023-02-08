#ifndef TP_IG_M2_S2_ABSTRACTLIGHT_H
#define TP_IG_M2_S2_ABSTRACTLIGHT_H

#include <QObject>
#include <QVector3D>

class AbstractLight : public QObject {

public:
    AbstractLight();
    explicit AbstractLight(QVector3D color);
    ~AbstractLight() override;

    const QVector3D &color() const;

    float intensity() const;

protected:
    QVector3D _color;
    bool _enabled;
    float _intensity;
};


#endif //TP_IG_M2_S2_ABSTRACTLIGHT_H
