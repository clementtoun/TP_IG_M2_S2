#include "OpenGLMaterial.h"

struct ShaderMaterialInfo {
    QVector4D color;      // 16          // 0
    float metallic;        // 4           // 16
    float roughness;        // 4           // 20
    int useColorMap;    // 4           // 32
    int useMetallicRoughnessMap;   // 4           // 36
};

static ShaderMaterialInfo shaderMaterialInfo;

OpenGLUniformBufferObject *OpenGLMaterial::_materialInfo = 0;

OpenGLMaterial::OpenGLMaterial(Material *material, QObject *parent) {
    _host = material;

    this->colorTextureChange(_host->getColorTexture());
    this->metallicRoughnessTextureChange(_host->getMetallicRoughnessTexture());

    setParent(parent);
}

Material *OpenGLMaterial::host() const {
    return _host;
}

void OpenGLMaterial::bind() {
    shaderMaterialInfo.useColorMap = false;
    shaderMaterialInfo.useMetallicRoughnessMap = false;

    if (_openGLColorTexture)
        shaderMaterialInfo.useColorMap = _openGLColorTexture->bind();
    if (_openGLMetallicRoughnessTexture)
        shaderMaterialInfo.useMetallicRoughnessMap = _openGLMetallicRoughnessTexture->bind();

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
