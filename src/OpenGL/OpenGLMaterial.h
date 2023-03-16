#ifndef TP_IG_M2_S2_OPENGLMATERIAL_H
#define TP_IG_M2_S2_OPENGLMATERIAL_H

#include <QObject>
#include "OpenGLTexture.h"
#include "OpenGLUniformBufferObject.h"
#include "../Scene/Material.h"

class OpenGLMaterial : public QObject {
    Q_OBJECT

public:
    explicit OpenGLMaterial(Material* material, QObject* parent = 0);

    ~OpenGLMaterial() override;

    Material* host() const;

    void bind();
    void release();

private slots:
    void colorTextureChange(QSharedPointer<Texture> texture);
    void metallicRoughnessTextureChange(QSharedPointer<Texture> texture);
    void normalTextureChange(QSharedPointer<Texture> texture);
    void aoTextureChange(QSharedPointer<Texture> texture);
    void emissiveTextureChange(QSharedPointer<Texture> texture);

private:
    Material* _host;
    OpenGLTexture* _openGLColorTexture, *_openGLMetallicRoughnessTexture, *_openGLNormalTexture, *_openGLAoTexture, *_openGLEmissiveTexture;
    static OpenGLUniformBufferObject *_materialInfo;

};


#endif //TP_IG_M2_S2_OPENGLMATERIAL_H
