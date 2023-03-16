#include "OpenGLMaterial.h"

struct ShaderMaterialInfo {
    QVector4D color;      // 16          // 0
    float metallic;        // 4           // 16
    float roughness;        // 4           // 20
    int useColorMap;    // 4           // 32
    int useMetallicRoughnessMap;   // 4           // 36
    int useNormalTexture;
    int useAoTexture;
    int useEmissiveTexture;
};

static ShaderMaterialInfo shaderMaterialInfo;

OpenGLUniformBufferObject *OpenGLMaterial::_materialInfo = 0;

OpenGLMaterial::OpenGLMaterial(Material *material, QObject *parent) {
    _host = material;

    this->colorTextureChange(_host->getColorTexture());
    this->metallicRoughnessTextureChange(_host->getMetallicRoughnessTexture());
    this->normalTextureChange(_host->getNormalTexture());
    this->aoTextureChange(_host->getAoTexture());
    this->emissiveTextureChange(_host->getEmissiveTexture());

    setParent(parent);
}

Material *OpenGLMaterial::host() const {
    return _host;
}

void OpenGLMaterial::bind() {
    shaderMaterialInfo.useColorMap = false;
    shaderMaterialInfo.useMetallicRoughnessMap = false;
    shaderMaterialInfo.useNormalTexture = false;
    shaderMaterialInfo.useAoTexture = false;
    shaderMaterialInfo.useEmissiveTexture = false;

    if (_openGLColorTexture)
        shaderMaterialInfo.useColorMap = _openGLColorTexture->bind();
    if (_openGLMetallicRoughnessTexture)
        shaderMaterialInfo.useMetallicRoughnessMap = _openGLMetallicRoughnessTexture->bind();
    if (_openGLNormalTexture)
        shaderMaterialInfo.useNormalTexture = _openGLNormalTexture->bind();
    if (_openGLAoTexture)
        shaderMaterialInfo.useAoTexture = _openGLAoTexture->bind();
    if (_openGLEmissiveTexture)
        shaderMaterialInfo.useEmissiveTexture = _openGLEmissiveTexture->bind();

    shaderMaterialInfo.color = _host->getColor();
    shaderMaterialInfo.metallic = _host->getMetallic();
    shaderMaterialInfo.roughness = _host->getRoughness();

    if (_materialInfo == 0) {
        _materialInfo = new OpenGLUniformBufferObject;
        _materialInfo->create();
        _materialInfo->bind();
        _materialInfo->allocate(MATERIAL_INFO_BINDING_POINT, NULL, sizeof(ShaderMaterialInfo));
        _materialInfo->release();
    }

    _materialInfo->bind();
    _materialInfo->write(0, &shaderMaterialInfo, sizeof(ShaderMaterialInfo));
    _materialInfo->release();
}

void OpenGLMaterial::release() {
    if (_openGLColorTexture) _openGLColorTexture->release();
    if (_openGLMetallicRoughnessTexture) _openGLMetallicRoughnessTexture->release();
    if (_openGLNormalTexture) _openGLNormalTexture->release();
    if (_openGLAoTexture) _openGLAoTexture->release();
    if (_openGLEmissiveTexture) _openGLEmissiveTexture->release();

    shaderMaterialInfo.color = QVector3D(0, 0, 0);
    shaderMaterialInfo.useColorMap = 0;
    shaderMaterialInfo.useMetallicRoughnessMap = 0;

    if (_materialInfo == 0) {
        _materialInfo = new OpenGLUniformBufferObject;
        _materialInfo->create();
        _materialInfo->bind();
        _materialInfo->allocate(3, NULL, sizeof(ShaderMaterialInfo));
        _materialInfo->release();
    }
    _materialInfo->bind();
    _materialInfo->write(0, &shaderMaterialInfo, sizeof(ShaderMaterialInfo));
    _materialInfo->release();
}

void OpenGLMaterial::colorTextureChange(QSharedPointer<Texture> texture) {
    if (texture.isNull())
        _openGLColorTexture = 0;
    else
        _openGLColorTexture = new OpenGLTexture(texture.data());
}

void OpenGLMaterial::metallicRoughnessTextureChange(QSharedPointer<Texture> texture) {
    if (texture.isNull())
        _openGLMetallicRoughnessTexture = 0;
    else
        _openGLMetallicRoughnessTexture = new OpenGLTexture(texture.data());
}

OpenGLMaterial::~OpenGLMaterial() {
    delete _openGLColorTexture;
    delete _openGLMetallicRoughnessTexture;
}

void OpenGLMaterial::normalTextureChange(QSharedPointer<Texture> texture) {
    if (texture.isNull())
        _openGLNormalTexture = 0;
    else
        _openGLNormalTexture = new OpenGLTexture(texture.data());
}

void OpenGLMaterial::aoTextureChange(QSharedPointer<Texture> texture) {
    if (texture.isNull())
        _openGLAoTexture = 0;
    else
        _openGLAoTexture = new OpenGLTexture(texture.data());
}

void OpenGLMaterial::emissiveTextureChange(QSharedPointer<Texture> texture) {
    if (texture.isNull())
        _openGLEmissiveTexture = 0;
    else
        _openGLEmissiveTexture = new OpenGLTexture(texture.data());
}
