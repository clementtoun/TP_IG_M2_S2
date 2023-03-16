
#ifndef TP_IG_M2_S2_OPENGLMESH_H
#define TP_IG_M2_S2_OPENGLMESH_H


#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QOpenGLFunctions_4_5_Core>
#include "../Scene/Mesh.h"
#include "OpenGLUniformBufferObject.h"
#include "OpenGLMaterial.h"

class OpenGLMesh : public QObject {
    Q_OBJECT
public:
    explicit OpenGLMesh(Mesh* mesh, QObject* parent = 0);
    ~OpenGLMesh() override;

    Mesh *host() const;

    void create();
    void commit();
    void render();
    void destroy();

private slots:
    void materialChanged(Material* material);

private:
    Mesh* _host;
    QOpenGLVertexArrayObject * _vao;
    QOpenGLBuffer * _vbo, *_ebo;
    QOpenGLFunctions_4_5_Core * glFuncs;
    OpenGLMaterial *_openGLMaterial;
    MeshType _type;

    static OpenGLUniformBufferObject *_modelInfo;
};


#endif //TP_IG_M2_S2_OPENGLMESH_H
