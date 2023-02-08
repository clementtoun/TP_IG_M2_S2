#ifndef TP_IG_M2_S2_OPENGLTEXTURE_H
#define TP_IG_M2_S2_OPENGLTEXTURE_H

#include "../Scene/Texture.h"
#include <QObject>
#include <QOpenGLTexture>

class OpenGLTexture: public QObject {
    Q_OBJECT

public:
    explicit OpenGLTexture(Texture* texture);
    ~OpenGLTexture() override;

    virtual void create();
    virtual bool bind();
    virtual void release();

private:
    Texture* _host;
    QOpenGLTexture *_openGLTexture;

private slots:
    void imageChanged(const QImage& image);
};


#endif //TP_IG_M2_S2_OPENGLTEXTURE_H
