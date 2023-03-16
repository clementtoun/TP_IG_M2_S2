#ifndef TP_IG_M2_S2_ABSTRACTENTITY_H
#define TP_IG_M2_S2_ABSTRACTENTITY_H

#include <QObject>
#include <QVector3D>
#include <QMatrix4x4>
#include <QQuaternion>

class AbstractEntity : public QObject {
    Q_OBJECT

public:
    explicit AbstractEntity(QObject * parent = 0);
    ~AbstractEntity() override;

    virtual void translate(QVector3D delta);
    virtual void rotate(QQuaternion rotation);
    virtual void rotate(QVector3D rotation);
    virtual void scale(QVector3D scaling);

    void setLocalModel(const QMatrix4x4 &model);

    QMatrix4x4 localModelMatrix() const;
    QMatrix4x4 globalModelMatrix() const;

    bool isLocked() const;
    void setLocked(bool locked);
    float getMass() const;
    void setMass(float mass);

    const QVector3D &getVitesse() const;

    void setVitesse(const QVector3D &vitesse);

private:
    QVector3D _position;
public:
    const QVector3D &getPosition() const;

    void setPosition(const QVector3D &position);

    const QVector3D &getRotation() const;

    void setRotation(const QVector3D &rotation);

    const QVector3D &getTranslation() const;

    void setTranslation(const QVector3D &translation);

private:
    QVector3D _rotation;
    QVector3D _translation;
    QVector3D _scale;
    QVector3D _vitesse;
    float _mass;
    bool _locked;
};


#endif //TP_IG_M2_S2_ABSTRACTENTITY_H
