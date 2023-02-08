#include "ModelLoader.h"
#include <iostream>
#include <QFileInfo>
#include <QMatrix4x4>

ModelLoader::ModelLoader() {}

Model *ModelLoader::loadModelFromFile(QString filePath) {
    Assimp::Importer import;
    const aiScene *scene = import.ReadFile(filePath.toStdString(), aiProcess_Triangulate);

    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
        return nullptr;
    }

    _dir = QFileInfo(filePath).absoluteDir();
    _aiScene = scene;

    Model* model = loadModel(scene->mRootNode);
    model->setObjectName(QFileInfo(filePath).baseName());

    return model;
}

Model *ModelLoader::loadModel(const aiNode *node) {
    Model* model = new Model();
    model->setObjectName(node->mName.length ? node->mName.C_Str() : "Untitled");

    auto mat = node->mTransformation;
    model->setLocalModel({mat[0][0], mat[0][1], mat[0][2], mat[0][3],
                          mat[1][0], mat[1][1], mat[1][2], mat[1][3],
                          mat[2][0], mat[2][1], mat[2][2], mat[2][3],
                          mat[3][0], mat[3][1], mat[3][2], mat[3][3]});

//    for(int i = 0; i < 4; i++) {
//        std::cout
//                << model->localModelMatrix().column(i).x()
//                << model->localModelMatrix().column(i).y()
//                << model->localModelMatrix().column(i).z()
//                << model->localModelMatrix().column(i).w()
//                << std::endl;
//    }

    for (unsigned int i = 0; i < node->mNumMeshes; i++)
        model->addChildMesh(loadMesh(_aiScene->mMeshes[node->mMeshes[i]]));
    for (unsigned int i = 0; i < node->mNumChildren; i++)
        model->addChildModel(loadModel(node->mChildren[i]));

    return model;
}

Mesh *ModelLoader::loadMesh(const aiMesh *aiMesh) {
    Mesh* mesh = new Mesh;
    mesh->setObjectName(aiMesh->mName.length ? aiMesh->mName.C_Str() : "Untitled");

    for(unsigned int i = 0; i < aiMesh->mNumVertices; i++)
    {
        Vertex vertex;
        if(aiMesh->HasPositions())
            vertex.position = QVector3D(aiMesh->mVertices[i].x, aiMesh->mVertices[i].y, aiMesh->mVertices[i].z);
        if(aiMesh->HasNormals())
            vertex.normal = QVector3D(aiMesh->mNormals[i].x, aiMesh->mNormals[i].y, aiMesh->mNormals[i].z);
        if(aiMesh->HasTextureCoords(0))
            vertex.texCoords = QVector2D(aiMesh->mTextureCoords[0][i].x, aiMesh->mTextureCoords[0][i].y);
        mesh->_vertices.push_back(vertex);
    }

    for(unsigned int i = 0; i < aiMesh->mNumFaces; i++)
    {
        aiFace face = aiMesh->mFaces[i];
        for(unsigned int j = 0; j < face.mNumIndices; j++)
            mesh->_indices.push_back(face.mIndices[j]);
    }

    mesh->setMaterial(loadMaterial(_aiScene->mMaterials[aiMesh->mMaterialIndex]));

    return mesh;
}

Material *ModelLoader::loadMaterial(const aiMaterial *aiMaterialPtr) {
    Material* material = new Material;
    aiColor4D color; float value; aiString aiStr;

    if (AI_SUCCESS == aiMaterialPtr->Get(AI_MATKEY_NAME, aiStr))
        material->setObjectName(aiStr.length ? aiStr.C_Str() : "Untitled");
    if (AI_SUCCESS == aiMaterialPtr->Get(AI_MATKEY_BASE_COLOR, color))
        material->setColor(QVector3D(color.r, color.g, color.b));
    if (AI_SUCCESS == aiMaterialPtr->Get(AI_MATKEY_METALLIC_FACTOR, value))
        material->setMetallic(value);
    if (AI_SUCCESS == aiMaterialPtr->GetTexture(AI_MATKEY_BASE_COLOR_TEXTURE, &aiStr)) {
        QString filePath = _dir.absolutePath() + '/' + QString(aiStr.C_Str()).replace('\\', '/');
        material->setColorTexture(_textureLoader.loadFromFile(Texture::Albedo, filePath));
    }
    if (AI_SUCCESS == aiMaterialPtr->GetTexture(AI_MATKEY_METALLIC_TEXTURE, &aiStr)) {
        QString filePath = _dir.absolutePath() + '/' + QString(aiStr.C_Str()).replace('\\', '/');
        material->setMetallicRoughnessTexture(_textureLoader.loadFromFile(Texture::MetalRoughness, filePath));
    }

    return material;
}
