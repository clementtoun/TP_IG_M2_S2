#include "ModelLoader.h"
#include <iostream>
#include <QFileInfo>
#include <QMatrix4x4>

ModelLoader::ModelLoader() {}

Model *ModelLoader::loadModelFromFile(QString filePath) {
    Assimp::Importer import;
    import.SetPropertyFloat("PP_GSN_MAX_SMOOTHING_ANGLE", 90);
    const aiScene *scene = import.ReadFile(filePath.toStdString(), aiProcess_Triangulate | aiProcess_CalcTangentSpace | aiProcess_FlipUVs);

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
    /*model->setLocalModel({mat[0][0], mat[1][0], mat[2][0], mat[3][0],
                          mat[0][1], mat[1][1], mat[2][1], mat[3][1],
                          mat[0][2], mat[1][2], mat[2][2], mat[3][2],
                          mat[0][3], mat[1][3], mat[2][3], mat[3][3]});*/

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
    Mesh* mesh = new Mesh(TRIANGLE_MESH);
    mesh->setObjectName(aiMesh->mName.length ? aiMesh->mName.C_Str() : "Untitled");

    AABB aabb;
    aabb.min = QVector4D(aiMesh->mVertices[0].x, aiMesh->mVertices[0].y, aiMesh->mVertices[0].z, 1.0);
    aabb.max = aabb.min;

    for(unsigned int i = 0; i < aiMesh->mNumVertices; i++)
    {
        Vertex vertex;
        if(aiMesh->HasPositions()) {
            vertex.position = QVector3D(aiMesh->mVertices[i].x, aiMesh->mVertices[i].y, aiMesh->mVertices[i].z);
            if(vertex.position.x() < aabb.min.x())
                aabb.min.setX(vertex.position.x());
            if(vertex.position.y() < aabb.min.y())
                aabb.min.setY(vertex.position.y());
            if(vertex.position.z() < aabb.min.z())
                aabb.min.setZ(vertex.position.z());
            if(vertex.position.x() > aabb.max.x())
                aabb.max.setX(vertex.position.x());
            if(vertex.position.y() > aabb.max.y())
                aabb.max.setY(vertex.position.y());
            if(vertex.position.z() > aabb.max.z())
                aabb.max.setZ(vertex.position.z());
        }
        if(aiMesh->HasNormals())
            vertex.normal = QVector3D(aiMesh->mNormals[i].x, aiMesh->mNormals[i].y, aiMesh->mNormals[i].z);
        if(aiMesh->HasTextureCoords(0))
            vertex.texCoords = QVector2D(aiMesh->mTextureCoords[0][i].x, aiMesh->mTextureCoords[0][i].y);
        if(aiMesh->HasTangentsAndBitangents()) {
            vertex.tangent = QVector3D(aiMesh->mTangents[i].x, aiMesh->mTangents[i].y, aiMesh->mTangents[i].z);
            vertex.bitangent = QVector3D(aiMesh->mBitangents[i].x, aiMesh->mBitangents[i].y, aiMesh->mBitangents[i].z);
        }
        mesh->_vertices.push_back(vertex);
    }

    mesh->_aabb = aabb;

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
    if (AI_SUCCESS == aiMaterialPtr->GetTexture(aiTextureType_NORMALS, 0, &aiStr)) {
        QString filePath = _dir.absolutePath() + '/' + QString(aiStr.C_Str()).replace('\\', '/');
        material->setNormalTexture(_textureLoader.loadFromFile(Texture::NORMALS, filePath));
    }
    if (AI_SUCCESS == aiMaterialPtr->GetTexture(aiTextureType_LIGHTMAP, 0, &aiStr)) {
        QString filePath = _dir.absolutePath() + '/' + QString(aiStr.C_Str()).replace('\\', '/');
        material->setAoTexture(_textureLoader.loadFromFile(Texture::AO, filePath));
    }
    if (AI_SUCCESS == aiMaterialPtr->GetTexture(aiTextureType_EMISSIVE, 0, &aiStr)) {
        QString filePath = _dir.absolutePath() + '/' + QString(aiStr.C_Str()).replace('\\', '/');
        material->setEmissiveTexture((_textureLoader.loadFromFile(Texture::EMISSIVE, filePath)));
    }

    return material;
}

Mesh *ModelLoader::loadMeshFromFile(QString filePath) {
    Assimp::Importer import;
    const aiScene *scene = import.ReadFile(filePath.toStdString(), aiProcess_Triangulate | aiProcess_CalcTangentSpace | aiProcess_FlipUVs);

    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
        return nullptr;
    }

    _dir = QFileInfo(filePath).absoluteDir();
    _aiScene = scene;

    Mesh* mesh = loadMesh(scene->mMeshes[0]);
    mesh->setObjectName(QFileInfo(filePath).baseName());
    mesh->setMaterial(new Material());

    return mesh;
}
