#ifndef TP_IG_M2_S2_VERTEX_H
#define TP_IG_M2_S2_VERTEX_H

#include <QVector3D>
#include <QVector2D>

struct Vertex {
    QVector3D position;
    QVector3D normal;
    QVector2D texCoords;
    QVector3D tangent;
    QVector3D bitangent;
};

#endif //TP_IG_M2_S2_VERTEX_H
