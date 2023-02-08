#ifndef TP_IG_M2_S2_OPENGLUNIFORMBUFFEROBJECT_H
#define TP_IG_M2_S2_OPENGLUNIFORMBUFFEROBJECT_H

#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLShaderProgram>

#define CAMERA_INFO_BINDING_POINT 0
#define MODEL_INFO_BINDING_POINT 1
#define MATERIAL_INFO_BINDING_POINT 2
#define LIGHT_INFO_BINDING_POINT 3

class OpenGLUniformBufferObject {

public:
    OpenGLUniformBufferObject();
    ~OpenGLUniformBufferObject();

    GLuint bufferId() const;

    bool create();
    void allocate(int indx, const void *data, int count) const;
    void destroy();

    void bind() const;
    void write(int offset, const void* data, int count) const;
    static void release();

    static void bindUniformBlock(QOpenGLShaderProgram * shader);

private:
    GLuint _id;
    static QOpenGLFunctions_3_3_Core* glFuncs;
};

#endif //TP_IG_M2_S2_OPENGLUNIFORMBUFFEROBJECT_H
