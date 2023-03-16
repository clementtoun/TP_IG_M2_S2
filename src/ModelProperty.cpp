#include "ModelProperty.h"

#include <QGridLayout>

ModelProperty::ModelProperty(Model * model, QWidget * parent): QWidget(parent) {
    _host = model;

    _visibleCheckBox = new QCheckBox("Visible", this);
    auto state = _host->isVisible() ? Qt::Checked : Qt::Unchecked;
    _visibleCheckBox->setCheckState(state);

    _lockedCheckBox = new QCheckBox("Locked", this);
    state = _host->isLocked() ? Qt::Checked : Qt::Unchecked;
    _lockedCheckBox->setCheckState(state);

    QVector3D position = _host->getPosition();
    _x = new QDoubleSpinBox(this);
    _x->setMinimum(-9999);
    _x->setMaximum(9999);
    _x->setValue(position.x());
    _y = new QDoubleSpinBox(this);
    _y->setMinimum(-9999);
    _y->setMaximum(9999);
    _y->setValue(position.y());
    _z = new QDoubleSpinBox(this);
    _z->setMinimum(-9999);
    _z->setMaximum(9999);
    _z->setValue(position.z());

    configLayout();
    configSignals();
}

void ModelProperty::configLayout() {
    auto * subLayout = new QGridLayout;
    subLayout->setAlignment(Qt::AlignTop);
    subLayout->setVerticalSpacing(10);
    subLayout->addWidget(_visibleCheckBox, 0, 0, 1, 2);
    subLayout->addWidget(_lockedCheckBox, 1, 0, 1, 2);
    subLayout->addWidget(_x, 4, 0, 1, 2);
    subLayout->addWidget(_y, 5, 0, 1, 2);
    subLayout->addWidget(_z, 6, 0, 1, 2);

    setLayout(subLayout);
}

void ModelProperty::configSignals() {
    connect(_x, SIGNAL(valueChanged(double)), this, SLOT(applyPosition()));
    connect(_y, SIGNAL(valueChanged(double)), this, SLOT(applyPosition()));
    connect(_z, SIGNAL(valueChanged(double)), this, SLOT(applyPosition()));
    connect(_visibleCheckBox, SIGNAL(stateChanged(int)), this, SLOT(setVisible(int)));
    connect(_lockedCheckBox, SIGNAL(stateChanged(int)), this, SLOT(setLocked(int)));
}

void ModelProperty::applyPosition() {
    _host->setPosition(QVector3D((float) _x->value(),(float) _y->value(),(float) _z->value()));
}

void ModelProperty::setVisible(int value) {
    _host->setVisible(value);
}

void ModelProperty::setLocked(int value) {
    _host->setLocked(value);
}

