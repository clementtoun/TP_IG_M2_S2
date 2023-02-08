
#include "EnvironmentLoader.h"

#include <utility>
#include "TextureLoader.h"

EnvironmentLoader::EnvironmentLoader() {}

Environment *EnvironmentLoader::loadFromFile(QString filePath) {
    auto* env = new Environment();

    TextureLoader loader;
    env->setEnvironmentMap(loader.loadFromFile(Texture::Albedo, std::move(filePath)));

    return env;
}
