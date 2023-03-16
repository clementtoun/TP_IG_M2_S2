
#include <QtMath>
#include <iostream>
#include "OpenGLRenderer.h"

OpenGLRenderer::OpenGLRenderer(QObject *parent) : QObject(nullptr) {
    _gltfShader = nullptr;
    _lightShader = nullptr;
    _cubeMapShader = nullptr;
    _CSMShader = nullptr;
    _shadowCubeShader = nullptr;
    _hdrShader = nullptr;
    glFuncs = nullptr;
    _shadowActive = true;
    setParent(parent);
}

bool OpenGLRenderer::reloadShaders() {

    delete _gltfShader;
    _gltfShader = nullptr;

    _gltfShader = new QOpenGLShaderProgram();
    _gltfShader->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, "../src/Shader/Gltf_Vert.shader");
    _gltfShader->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment,"../src/Shader/Gltf_Frag.shader");
    _gltfShader->link();

    OpenGLUniformBufferObject::bindUniformBlock(_gltfShader);

    _gltfShader->bind();
    _gltfShader->setUniformValue("colorTexture", 0);
    _gltfShader->setUniformValue("metallicRoughnessTexture", 1);
    _gltfShader->setUniformValue("normalTexture", 2);
    _gltfShader->setUniformValue("aoTexture", 3);
    _gltfShader->setUniformValue("emissiveTexture", 4);
    _gltfShader->setUniformValue("shadowMap", 5);
    _gltfShader->setUniformValue("shadowCubeMap", 6);
    _gltfShader->setUniformValue("skybox", 7);

    delete _lightShader;
    _lightShader = nullptr;

    _lightShader = new QOpenGLShaderProgram();
    _lightShader->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, "../src/Shader/LightShader_Vert.shader");
    _lightShader->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, "../src/Shader/LightShader_Frag.shader");
    _lightShader->link();

    OpenGLUniformBufferObject::bindUniformBlock(_lightShader);

    delete _cubeMapShader;
    _cubeMapShader = nullptr;

    _cubeMapShader = new QOpenGLShaderProgram();
    _cubeMapShader->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, "../src/Shader/cubeMap_Vert.shader");
    _cubeMapShader->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, "../src/Shader/cubeMap_Frag.shader");
    _cubeMapShader->link();

    OpenGLUniformBufferObject::bindUniformBlock(_cubeMapShader);

    _cubeMapShader->bind();
    _cubeMapShader->setUniformValue("skybox", 7);

    delete _CSMShader;
    _CSMShader = nullptr;

    _CSMShader = new QOpenGLShaderProgram();
    _CSMShader->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, "../src/Shader/CSM_Vert.shader");
    _CSMShader->addCacheableShaderFromSourceFile(QOpenGLShader::Geometry, "../src/Shader/CSM_Geom.shader");
    _CSMShader->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, "../src/Shader/CSM_Frag.shader");
    _CSMShader->link();

    OpenGLUniformBufferObject::bindUniformBlock(_CSMShader);

    delete _shadowCubeShader;
    _shadowCubeShader = nullptr;

    _shadowCubeShader = new QOpenGLShaderProgram();
    _shadowCubeShader->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, "../src/Shader/PointShadowDepthMap_Vert.shader");
    _shadowCubeShader->addCacheableShaderFromSourceFile(QOpenGLShader::Geometry, "../src/Shader/PointShadowDepthMap_Geom.shader");
    _shadowCubeShader->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, "../src/Shader/PointShadowDepthMap_Frag.shader");
    _shadowCubeShader->link();

    OpenGLUniformBufferObject::bindUniformBlock(_shadowCubeShader);

    delete _hdrShader;
    _hdrShader = nullptr;

    _hdrShader = new QOpenGLShaderProgram();
    _hdrShader->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, "../src/Shader/hdr_Vert.shader");
    _hdrShader->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, "../src/Shader/hdr_Frag.shader");
    _hdrShader->link();

    _hdrShader->bind();
    _hdrShader->setUniformValue("hdrBuffer", 0);

    return _gltfShader && _cubeMapShader && _lightShader && _CSMShader && _hdrShader;
}

void OpenGLRenderer::render(OpenGLScene *openGLScene) {
    glFuncs->glBindFramebuffer(GL_FRAMEBUFFER, _hdrFBO);
    glFuncs->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    bool has_cubeMap = openGLScene->hasEnvironment();

    if(_cubeMapShader && has_cubeMap) {
        glFuncs->glBindFramebuffer(GL_FRAMEBUFFER, _hdrFBO);
        glFuncs->glViewport(0, 0, _width, _height);
        _cubeMapShader->bind();
        openGLScene->renderEnvironment();
        has_cubeMap = 1;
    }

    if(_gltfShader) {
        bool first_render = true;

        for (int i = 0; i < openGLScene->host()->directionalLights().size(); i++) {
            DirectionalLight *light = openGLScene->host()->directionalLights()[i];

            auto cam = openGLScene->host()->camera();
            const auto lightMatrices = getLightSpaceMatrices(*cam, light->direction());

            _CSMShader->bind();
            for(int k=0; k < lightMatrices.size(); ++k)
                _CSMShader->setUniformValue(("lightSpaceMatrices[" + std::to_string(k) + "]").c_str(), lightMatrices.at(k));

            glFuncs->glBindFramebuffer(GL_FRAMEBUFFER, _depthMapFBO);
            glFuncs->glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
            glFuncs->glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, _depthCSMap, 0);
            glFuncs->glDrawBuffer(GL_NONE);
            glFuncs->glReadBuffer(GL_NONE);
            glFuncs->glClear(GL_DEPTH_BUFFER_BIT);
            glFuncs->glCullFace(GL_FRONT);
            openGLScene->renderModels();
            glFuncs->glCullFace(GL_BACK);

            glFuncs->glBindFramebuffer(GL_FRAMEBUFFER, _hdrFBO);
            glFuncs->glViewport(0, 0, _width, _height);

            glFuncs->glEnable(GL_BLEND);
            if (first_render) {
                glFuncs->glBlendFunc(GL_ONE, GL_ZERO);
                first_render = false;
            } else {
                glFuncs->glDepthFunc(GL_LEQUAL);
                glFuncs->glBlendFunc(GL_ONE, GL_ONE);
            }
            openGLScene->commitLightInfo(light);
            _gltfShader->bind();
            _gltfShader->setUniformValue("hasCubeMap", has_cubeMap);
            _gltfShader->setUniformValue("shadowActive", _shadowActive);
            for(int k = 0; k < lightMatrices.size(); ++k)
                _gltfShader->setUniformValue(("lightSpaceMatrices[" + std::to_string(k) + "]").c_str(), lightMatrices.at(k));
            _gltfShader->setUniformValue("cascadeCount", _shadowCascadeLevels.size());
            for (int k = 0; k < _shadowCascadeLevels.size(); ++k)
                _gltfShader->setUniformValue(("cascadePlaneDistances[" + std::to_string(k) + "]").c_str(), _shadowCascadeLevels.at(k));
            _gltfShader->setUniformValue("far_plane", cam->getFar());
            glFuncs->glActiveTexture(GL_TEXTURE5);
            glFuncs->glBindTexture(GL_TEXTURE_2D_ARRAY, _depthCSMap);
            openGLScene->renderModels();

            glFuncs->glDisable(GL_BLEND);
            glFuncs->glDepthFunc(GL_LESS);
        }

        for(int i = 0; i < openGLScene->host()->pointLights().size(); i++) {
            PointLight *light = openGLScene->host()->pointLights()[i];

            float aspect = (float)SHADOW_WIDTH/(float)SHADOW_HEIGHT;
            float near = 0.1f;
            float far = 50.0f;
            QMatrix4x4 shadowProj;
            shadowProj.perspective(90.0f, aspect, near, far);
            QMatrix4x4 lightView;
            QVector<QMatrix4x4> shadowTransforms;
            lightView.lookAt(light->position(), light->position() + QVector3D( 1.0, 0.0, 0.0), QVector3D(0.0,-1.0, 0.0));
            shadowTransforms.push_back(shadowProj * lightView);
            lightView.setToIdentity();
            lightView.lookAt(light->position(), light->position() + QVector3D( -1.0, 0.0, 0.0), QVector3D(0.0,-1.0, 0.0));
            shadowTransforms.push_back(shadowProj * lightView);
            lightView.setToIdentity();
            lightView.lookAt(light->position(), light->position() + QVector3D( 0.0, 1.0, 0.0), QVector3D(0.0, 0.0, 1.0));
            shadowTransforms.push_back(shadowProj * lightView);
            lightView.setToIdentity();
            lightView.lookAt(light->position(), light->position() + QVector3D( 0.0, -1.0, 0.0), QVector3D(0.0, 0.0, -1.0));
            shadowTransforms.push_back(shadowProj * lightView);
            lightView.setToIdentity();
            lightView.lookAt(light->position(), light->position() + QVector3D( 0.0, 0.0, 1.0), QVector3D(0.0, -1.0, 0.0));
            shadowTransforms.push_back(shadowProj * lightView);
            lightView.setToIdentity();
            lightView.lookAt(light->position(), light->position() + QVector3D( 0.0, 0.0, -1.0), QVector3D(0.0, -1.0, 0.0));
            shadowTransforms.push_back(shadowProj * lightView);

            _shadowCubeShader->bind();
            for(int j = 0; j < shadowTransforms.size(); j++)
                _shadowCubeShader->setUniformValue(("shadowMatrices[" + std::to_string(j) + "]").c_str(), shadowTransforms.at(j));
            _shadowCubeShader->setUniformValue("lightPos", light->position());
            _shadowCubeShader->setUniformValue("far_plane", far);

            glFuncs->glBindFramebuffer(GL_FRAMEBUFFER, _depthMapFBO);
            glFuncs->glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
            glFuncs->glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, _depthCubeMap, 0);
            glFuncs->glDrawBuffer(GL_NONE);
            glFuncs->glReadBuffer(GL_NONE);
            glFuncs->glClear(GL_DEPTH_BUFFER_BIT);
            glFuncs->glCullFace(GL_FRONT);
            openGLScene->renderModels();
            glFuncs->glCullFace(GL_BACK);

            glFuncs->glBindFramebuffer(GL_FRAMEBUFFER, _hdrFBO);
            glFuncs->glViewport(0, 0, _width, _height);

            glFuncs->glEnable(GL_BLEND);
            if (first_render) {
                glFuncs->glBlendFunc(GL_ONE, GL_ZERO);
                first_render = false;
            } else {
                glFuncs->glDepthFunc(GL_LEQUAL);
                glFuncs->glBlendFunc(GL_ONE, GL_ONE);
            }
            openGLScene->commitLightInfo(light);
            _gltfShader->bind();
            _gltfShader->setUniformValue("hasCubeMap", has_cubeMap);
            _gltfShader->setUniformValue("shadowActive", _shadowActive);
            _gltfShader->setUniformValue("far_plane", far);
            glFuncs->glActiveTexture(GL_TEXTURE6);
            glFuncs->glBindTexture(GL_TEXTURE_CUBE_MAP, _depthCubeMap);
            openGLScene->renderModels();

            glFuncs->glDisable(GL_BLEND);
            glFuncs->glDepthFunc(GL_LESS);
        }
    }

    if(_lightShader) {
        glFuncs->glBindFramebuffer(GL_FRAMEBUFFER, _hdrFBO);
        glFuncs->glViewport(0, 0, _width, _height);
        _lightShader->bind();
        openGLScene->renderLights();
    }

    glFuncs->glBindFramebuffer(GL_FRAMEBUFFER, _context->defaultFramebufferObject());
    glFuncs->glViewport(0, 0, _width, _height);
    glFuncs->glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

    _hdrShader->bind();
    glFuncs->glActiveTexture(GL_TEXTURE0);
    glFuncs->glBindTexture(GL_TEXTURE_2D, _colorBuffer);
    glFuncs->glBindVertexArray(_quadVAO);
    glFuncs->glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glFuncs->glBindVertexArray(0);
}

OpenGLRenderer::~OpenGLRenderer() {
    delete _gltfShader;
    delete _cubeMapShader;
    delete _lightShader;
    delete _CSMShader;
    delete _shadowCubeShader;
    delete _hdrShader;

    glFuncs->glDeleteFramebuffers(1, &_depthMapFBO);
    glFuncs->glDeleteTextures(1, &_depthCSMap);
    glFuncs->glDeleteTextures(1, &_depthCubeMap);

    glFuncs->glDeleteFramebuffers(1, &_hdrFBO);
    glFuncs->glDeleteRenderbuffers(1, &_rboDepth);
    glFuncs->glDeleteTextures(1, &_colorBuffer);

    glFuncs->glDeleteVertexArrays(1, &_quadVAO);
    glFuncs->glDeleteBuffers(1, &_quadVBO);
}

void OpenGLRenderer::initialiseShadowFrameBuffer() {
    if(!glFuncs)
        glFuncs = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_4_5_Core>();

    glFuncs->glGenFramebuffers(1, &_depthMapFBO);

    glFuncs->glGenTextures(1, &_depthCSMap);
    glFuncs->glBindTexture(GL_TEXTURE_2D_ARRAY, _depthCSMap);
    glFuncs->glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_DEPTH_COMPONENT32F,
                 SHADOW_WIDTH, SHADOW_HEIGHT, int(_shadowCascadeLevels.size()) + 1, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glFuncs->glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glFuncs->glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFuncs->glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glFuncs->glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    constexpr float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glFuncs->glTexParameterfv(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BORDER_COLOR, borderColor);

    glFuncs->glBindFramebuffer(GL_FRAMEBUFFER, _depthCSMap);
    glFuncs->glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, _depthCSMap, 0);
    glFuncs->glDrawBuffer(GL_NONE);
    glFuncs->glReadBuffer(GL_NONE);

    glFuncs->glGenTextures(1, &_depthCubeMap);
    glFuncs->glBindTexture(GL_TEXTURE_CUBE_MAP, _depthCubeMap);
    for (unsigned int i = 0; i < 6; ++i)
        glFuncs->glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT,
                              SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glFuncs->glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFuncs->glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glFuncs->glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glFuncs->glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFuncs->glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

void OpenGLRenderer::initialiseHDR() {
    if(!glFuncs)
        glFuncs = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_4_5_Core>();

    glFuncs->glGenFramebuffers(1, &_hdrFBO);

    glFuncs->glGenTextures(1, &_colorBuffer);
    glFuncs->glBindTexture(GL_TEXTURE_2D, _colorBuffer);
    glFuncs->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F,
                          _width, _height, 0, GL_RGBA, GL_FLOAT, NULL);
    glFuncs->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glFuncs->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glFuncs->glGenRenderbuffers(1, &_rboDepth);
    glFuncs->glBindRenderbuffer(GL_RENDERBUFFER, _rboDepth);
    glFuncs->glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, _width, _height);

    glFuncs->glBindFramebuffer(GL_FRAMEBUFFER, _hdrFBO);
    glFuncs->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _colorBuffer, 0);
    glFuncs->glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, _rboDepth);
    if (glFuncs->glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << glFuncs->glCheckFramebufferStatus(GL_FRAMEBUFFER) << std::endl;

    glFuncs->glBindFramebuffer(GL_FRAMEBUFFER, _context->defaultFramebufferObject());

    float quadVertices[] = {
            // positions        // texture Coords
            -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
            1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
            1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
    };
    // setup plane VAO
    glFuncs->glGenVertexArrays(1, &_quadVAO);
    glFuncs->glGenBuffers(1, &_quadVBO);
    glFuncs->glBindVertexArray(_quadVAO);
    glFuncs->glBindBuffer(GL_ARRAY_BUFFER, _quadVBO);
    glFuncs->glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glFuncs->glEnableVertexAttribArray(0);
    glFuncs->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glFuncs->glEnableVertexAttribArray(1);
    glFuncs->glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glFuncs->glBindVertexArray(0);
}

void OpenGLRenderer::setContext(QOpenGLContext *context) {
    _context = context;
}

void OpenGLRenderer::setSize(unsigned int width, unsigned int height) {
    _width = width;
    _height = height;
    if(_quadVAO != 0) {
        glFuncs->glBindTexture(GL_TEXTURE_2D, _colorBuffer);
        glFuncs->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F,
                              _width, _height, 0, GL_RGBA, GL_FLOAT, NULL);
        glFuncs->glBindRenderbuffer(GL_RENDERBUFFER, _rboDepth);
        glFuncs->glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, _width, _height);
        glFuncs->glBindFramebuffer(GL_FRAMEBUFFER, _hdrFBO);
        glFuncs->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _colorBuffer, 0);
        glFuncs->glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, _rboDepth);
        glFuncs->glBindFramebuffer(GL_FRAMEBUFFER, _context->defaultFramebufferObject());
    }
}

QVector<QVector4D> getFrustumCornersWorldSpace(const QMatrix4x4 &proj, const QMatrix4x4 &view) {
    const auto inv = (proj * view).inverted();

    QVector<QVector4D> frustumCorners;
    for (unsigned int x = 0; x < 2; ++x)
    {
        for (unsigned int y = 0; y < 2; ++y)
        {
            for (unsigned int z = 0; z < 2; ++z)
            {
                const QVector4D pt =
                        inv * QVector4D(
                                2.0f * x - 1.0f,
                                2.0f * y - 1.0f,
                                2.0f * z - 1.0f,
                                1.0f);
                frustumCorners.push_back(pt / pt.w());
            }
        }
    }

    return frustumCorners;
}

QMatrix4x4 getLightSpaceMatrix(Camera& camera, const QVector3D& lightDir, const float nearPlane, const float farPlane) {
    QMatrix4x4 proj;
    proj.perspective(
            camera.zoom(), camera.getAspect(), nearPlane,
            farPlane);
    const auto corners = getFrustumCornersWorldSpace(proj, camera.viewmatrix());

    QVector3D center = QVector3D(0, 0, 0);
    for (const auto& v : corners)
    {
        center += QVector3D(v);
    }
    center /= (float) corners.size();

    QMatrix4x4 lightView;
    lightView.lookAt(center - lightDir.normalized(), center, QVector3D(0.0f, 1.0f, 0.0f));

    float minX = std::numeric_limits<float>::max();
    float maxX = std::numeric_limits<float>::lowest();
    float minY = std::numeric_limits<float>::max();
    float maxY = std::numeric_limits<float>::lowest();
    float minZ = std::numeric_limits<float>::max();
    float maxZ = std::numeric_limits<float>::lowest();
    for (const auto& v : corners)
    {
        const auto trf = lightView * v;
        minX = std::min(minX, trf.x());
        maxX = std::max(maxX, trf.x());
        minY = std::min(minY, trf.y());
        maxY = std::max(maxY, trf.y());
        minZ = std::min(minZ, trf.z());
        maxZ = std::max(maxZ, trf.z());
    }

    // Tune this parameter according to the scene
    constexpr float zMult = 10.0f;
    if (minZ < 0)
    {
        minZ *= zMult;
    }
    else
    {
        minZ /= zMult;
    }
    if (maxZ < 0)
    {
        maxZ /= zMult;
    }
    else
    {
        maxZ *= zMult;
    }

    QMatrix4x4 lightProjection;
    lightProjection.ortho(minX, maxX, minY, maxY, minZ, maxZ);

    return lightProjection * lightView;
}

QVector<QMatrix4x4> OpenGLRenderer::getLightSpaceMatrices(Camera& camera, const QVector3D& lightDir) {
    QVector<QMatrix4x4> ret;
    for (int i = 0; i < _shadowCascadeLevels.size() + 1; ++i)
    {
        if (i == 0)
        {
            ret.push_back(getLightSpaceMatrix(camera, lightDir, camera.getNear(), _shadowCascadeLevels[i]));
        }
        else if (i < _shadowCascadeLevels.size())
        {
            ret.push_back(getLightSpaceMatrix(camera, lightDir, _shadowCascadeLevels[i - 1], _shadowCascadeLevels[i]));
        }
        else
        {
            ret.push_back(getLightSpaceMatrix(camera, lightDir, _shadowCascadeLevels[i - 1], camera.getFar()));
        }
    }
    return ret;
}

void OpenGLRenderer::setShadowCascadeLevels(const float cameraFarPlane) {
    _shadowCascadeLevels = {cameraFarPlane / 50.0f, cameraFarPlane / 25.0f, cameraFarPlane / 10.0f, cameraFarPlane / 2.0f};
}

bool OpenGLRenderer::isShadowActive() const {
    return _shadowActive;
}

void OpenGLRenderer::setShadowActive(bool shadowActive) {
    _shadowActive = shadowActive;
}
