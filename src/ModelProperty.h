#ifndef TP_IG_M2_S2_MODELPROPERTY_H
#define TP_IG_M2_S2_MODELPROPERTY_H


#include <QWidget>
#include <QCheckBox>
#include <QDoubleSpinBox>
#include "Scene/Model.h"

class ModelProperty : public QWidget {
    Q_OBJECT

public:
    explicit ModelProperty(Model * model, QWidget * parent = 0);

public slots:
    void applyPosition();
    void setVisible(int value);
    void setLocked(int value);

private:
    Model *_host;
    QCheckBox *_visibleCheckBox;
    QCheckBox *_lockedCheckBox;
    QDoubleSpinBox *_x, *_y, *_z;

    void configLayout();
    void configSignals();
};


#endif //TP_IG_M2_S2_MODELPROPERTY_H
