
#include <QtMath>
#include "OpenGLRenderer.h"

OpenGLRenderer::OpenGLRenderer(QObject *parent) : QObject(nullptr) {
    _basicShader = nullptr;
    glFuncs = nullptr;
    setParent(parent);
}

bool OpenGLRenderer::reloadShaders() {
    delete _basicShader;
    _basicShader = nullptr;

    _basicShader = new QOpenGLShaderProgram();
    _basicShader->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, "../src/Shader/Gltf_Vert.shader");
    _basicShader->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment,"../src/Shader/Gltf_Frag.shader");
    _basicShader->link();

    OpenGLUniformBufferObject::bindUniformBlock(_basicShader);

    _basicShader->bind();
    _basicShader->setUniformValue("colorTexture", 0);
    _basicShader->setUniformValue("metallicRoughnessTexture", 1);
    _basicShader->setUniformValue("shadowMap", 2);
    _basicShader->setUniformValue("shadowCubeMap", 3);

    _lightShader = new QOpenGLShaderProgram();
    _lightShader->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, "../src/Shader/LightShader_Vert.shader");
    _lightShader->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, "../src/Shader/LightShader_Frag.shader");
    _lightShader->link();

    OpenGLUniformBufferObject::bindUniformBlock(_lightShader);

    _cubeMapShader = new QOpenGLShaderProgram();
    _cubeMapShader->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, "../src/Shader/cubeMap_Vert.shader");
    _cubeMapShader->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, "../src/Shader/cubeMap_Frag.shader");
    _cubeMapShader->link();

    _cubeMapShader->bind();
    _cubeMapShader->setUniformValue("skybox", 0);

    _shadowMapShader = new QOpenGLShaderProgram();
    _shadowMapShader->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, "../src/Shader/ShadowDepthMap_Vert.shader");
    _shadowMapShader->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, "../src/Shader/ShadowDepthMap_Frag.shader");
    _shadowMapShader->link();

    OpenGLUniformBufferObject::bindUniformBlock(_shadowMapShader);

    _shadowCubeShader = new QOpenGLShaderProgram();
    _shadowCubeShader->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, "../src/Shader/PointShadowDepthMap_Vert.shader");
    _shadowCubeShader->addCacheableShaderFromSourceFile(QOpenGLShader::Geometry, "../src/Shader/PointShadowDepthMap_Geom.shader");
    _shadowCubeShader->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, "../src/Shader/PointShadowDepthMap_Frag.shader");
    _shadowCubeShader->link();

    OpenGLUniformBufferObject::bindUniformBlock(_shadowCubeShader);

    return _basicShader && _cubeMapShader && _lightShader && _shadowMapShader;
}

void OpenGLRenderer::render(OpenGLScene *openGLScene) {
    if(_basicShader) {
        bool first_render = true;

        for (int i = 0; i < openGLScene->host()->directionalLights().size(); i++) {
            DirectionalLight *light = openGLScene->host()->directionalLights()[i];

            float near_plane = 0.1f, far_plane = 50.0f;
            QMatrix4x4 lightProjection;
            lightProjection.ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
            QMatrix4x4 lightView;
            lightView.lookAt(-light->direction()*10, light->direction(), QVector3D(0, 1.0, 0.0));
            QMatrix4x4 lightSpaceMatrix = lightProjection * lightView;

            _shadowMapShader->bind();
            _shadowMapShader->setUniformValue("lightSpaceMatrix", lightSpaceMatrix);

            glFuncs->glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
            glFuncs->glBindFramebuffer(GL_FRAMEBUFFER, _depthMapFBO);
            glFuncs->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, _depthMap, 0);
            glFuncs->glDrawBuffer(GL_NONE);
            glFuncs->glReadBuffer(GL_NONE);
            glFuncs->glClear(GL_DEPTH_BUFFER_BIT);
            glCullFace(GL_FRONT);
            openGLScene->renderModels();
            glCullFace(GL_BACK);

            glFuncs->glBindFramebuffer(GL_FRAMEBUFFER, _context->defaultFramebufferObject());
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
            _basicShader->bind();
            _basicShader->setUniformValue("lightSpaceMatrix", lightSpaceMatrix);
            glFuncs->glActiveTexture(GL_TEXTURE2);
            glFuncs->glBindTexture(GL_TEXTURE_2D, _depthMap);
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

            glFuncs->glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
            glFuncs->glBindFramebuffer(GL_FRAMEBUFFER, _depthMapFBO);
            glFuncs->glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, _depthCubeMap, 0);
            glFuncs->glDrawBuffer(GL_NONE);
            glFuncs->glReadBuffer(GL_NONE);
            glFuncs->glClear(GL_DEPTH_BUFFER_BIT);
            openGLScene->renderModels();

            glFuncs->glBindFramebuffer(GL_FRAMEBUFFER, _context->defaultFramebufferObject());
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
            _basicShader->bind();
            _basicShader->setUniformValue("far_plane", far);
            glFuncs->glActiveTexture(GL_TEXTURE3);
            glFuncs->glBindTexture(GL_TEXTURE_CUBE_MAP, _depthCubeMap);
            openGLScene->renderModels();

            glFuncs->glDisable(GL_BLEND);
            glFuncs->glDepthFunc(GL_LESS);
        }
    }

    if(_lightShader) {
        _lightShader->bind();
        openGLScene->renderLights();
    }

    if(_cubeMapShader) {
        _cubeMapShader->bind();
        openGLScene->renderEnvironment();
    }

}

OpenGLRenderer::~OpenGLRenderer() {
    delete _basicShader;
    delete _cubeMapShader;
}

void OpenGLRenderer::initialiseShadowFrameBuffer() {
    if(!glFuncs)
        glFuncs = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_3_3_Core>();

    glFuncs->glGenFramebuffers(1, &_depthMapFBO);
    glFuncs->glGenTextures(1, &_depthMap);
    glFuncs->glBindTexture(GL_TEXTURE_2D, _depthMap);
    glFuncs->glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
                 SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glFuncs->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glFuncs->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFuncs->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glFuncs->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glFuncs->glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

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

void OpenGLRenderer::setWidth(unsigned int width) {
    _width = width;
}

void OpenGLRenderer::setHeight(unsigned int height) {
    _height = height;
}

void OpenGLRenderer::setContext(QOpenGLContext *context) {
    _context = context;
}
