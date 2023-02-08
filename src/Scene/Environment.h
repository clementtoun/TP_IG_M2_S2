#ifndef TP_IG_M2_S2_ENVIRONMENT_H
#define TP_IG_M2_S2_ENVIRONMENT_H

#include <QSharedPointer>
#include "Texture.h"

class Environment : public QObject {
    Q_OBJECT

public:
    explicit Environment(QObject * parent = 0);


    const QSharedPointer<Texture> &getEnvironmentMap() const;
    void setEnvironmentMap(const QSharedPointer<Texture> &environmentMap);

private:
    QSharedPointer<Texture> _environmentMap;
};


#endif //TP_IG_M2_S2_ENVIRONMENT_H
