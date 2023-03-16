#include "SceneTreeWidget.h"

SceneTreeWidget::SceneTreeWidget(QWidget *parent): QTreeWidget(parent) {
    setAnimated(true);
    setScene(0);
    setSingal();
}

SceneTreeWidget::SceneTreeWidget(Scene* scene, QWidget* parent): QTreeWidget(parent) {
    setAnimated(true);
    setScene(scene);
    setSingal();
}

void SceneTreeWidget::setSingal() {
    connect(this, SIGNAL(currentItemChanged(QTreeWidgetItem *, QTreeWidgetItem *)),
            this, SLOT(currentItemChanged(QTreeWidgetItem *, QTreeWidgetItem *)));
}

void SceneTreeWidget::setScene(Scene * scene) {
    m_host = scene;
    if (m_host) {
        connect(m_host, SIGNAL(lightAdded(AbstractLight*)), this, SLOT(lightAdded(AbstractLight*)));
        connect(m_host, SIGNAL(modelAdded(Model*)), this, SLOT(modelAdded(Model*)));
    }
    reload();
}

void SceneTreeWidget::reload() {
    this->clear();
    this->setColumnCount(1);
    this->setHeaderLabel("Scene");
    this->setCurrentItem(0);
    if (!m_host) return;

    for (int i = 0; i < m_host->models().size(); i++)
        new ModelItem(m_host->models()[i], invisibleRootItem());
}

void SceneTreeWidget::modelAdded(Model * model) {
    addTopLevelItem(new ModelItem(model, 0));
}

void SceneTreeWidget::lightAdded(AbstractLight *light) {

}

void SceneTreeWidget::currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous) {
    itemSelected(current->data(0, Qt::UserRole));
}
