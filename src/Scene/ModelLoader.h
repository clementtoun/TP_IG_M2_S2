#ifndef TP_IG_M2_S2_MODELLOADER_H
#define TP_IG_M2_S2_MODELLOADER_H

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include <QDir>
#include "Model.h"
#include "Material.h"
#include "TextureLoader.h"

class ModelLoader {
public:
    ModelLoader();

    Model* loadModelFromFile(QString filePath);
    Model* loadModel(const aiNode *node);
    Mesh *loadMesh(const aiMesh *aiMesh);

    Material* loadMaterial(const aiMaterial * aiMaterialPtr);

private:
    const aiScene* _aiScene;
    QDir _dir;
    TextureLoader _textureLoader;
};


#endif //TP_IG_M2_S2_MODELLOADER_H
