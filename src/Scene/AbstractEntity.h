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

    QMatrix4x4 _model;
};


#endif //TP_IG_M2_S2_ABSTRACTENTITY_H
