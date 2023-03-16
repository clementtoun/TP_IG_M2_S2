#ifndef TP_IG_M2_S2_SCENETREEWIDGET_H
#define TP_IG_M2_S2_SCENETREEWIDGET_H

#include "Scene/Scene.h"
#include "ModelProperty.h"
#include <QTreeWidget>

class SceneTreeWidget : public QTreeWidget {
    Q_OBJECT

public:
    explicit SceneTreeWidget(QWidget* parent = 0);
    explicit SceneTreeWidget(Scene* scene, QWidget* parent = 0);

    void setScene(Scene* scene);

    void setSingal();

public slots:
    void reload();
    void currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);

signals:
    void itemSelected(QVariant item);
    void itemDeselected(QVariant item);

private:
    Scene* m_host;

private slots:
    void lightAdded(AbstractLight* light);
    void modelAdded(Model* model);
};

class BaseItem: public QObject, public QTreeWidgetItem {
    Q_OBJECT

public:
    BaseItem(QObject* host, QTreeWidgetItem* parent): QObject(0), QTreeWidgetItem(parent) {
        setText(0, host->objectName());
        setData(0, Qt::UserRole, QVariant::fromValue(host));
        connect(host, SIGNAL(destroyed(QObject*)), this, SLOT(hostDestroyed(QObject*)));
    }

private slots:
    void hostDestroyed(QObject*) {
        delete this;
    }
};

class MeshItem: public BaseItem {
    Q_OBJECT

public:
    MeshItem(Mesh* host, QTreeWidgetItem* parent): BaseItem(host, parent) {
        m_host = host;
    }

private:
    Mesh* m_host;

private slots:
    void selectedChanged(bool selected) {
        if (selected) {
            if (!isExpanded())
                setExpanded(true);
            treeWidget()->setCurrentItem(this);
        } else
            treeWidget()->setCurrentItem(0);
    }
};

class ModelItem: public BaseItem {
Q_OBJECT

public:
    ModelItem(Model* host, QTreeWidgetItem* parent): BaseItem(host, parent) {
        m_host = host;
        for (int i = 0; i < m_host->childMeshes().size(); i++)
            new MeshItem(m_host->childMeshes()[i], this);
        for (int i = 0; i < m_host->childModels().size(); i++)
            new ModelItem(m_host->childModels()[i], this);
        connect(m_host, SIGNAL(childMeshAdded(Mesh*)), this, SLOT(childMeshAdded(Mesh*)));
        connect(m_host, SIGNAL(childModelAdded(Model*)), this, SLOT(childModelAdded(Model*)));
    }

private:
    Model* m_host;

private slots:
    void selectedChanged(bool selected) {
        if (selected) {
            if (!isExpanded())
                setExpanded(true);
            treeWidget()->setCurrentItem(this);
        } else
            treeWidget()->setCurrentItem(0);
    }
    void childMeshAdded(Mesh* mesh) {
        new MeshItem(mesh, this);
    }
    void childModelAdded(Model* model) {
        new ModelItem(model, this);
    }
};



#endif //TP_IG_M2_S2_SCENETREEWIDGET_H
