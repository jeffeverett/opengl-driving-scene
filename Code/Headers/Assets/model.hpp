#pragma once

#include "Assets/mesh.hpp"
#include "Assets/texture.hpp"

#include <assimp/scene.h>

#include <memory>
#include <string>
#include <vector>
#include <set>

namespace Assets
{
    class Model
    {
    public:
        Model(std::string const &path);

        std::vector<std::shared_ptr<Mesh>> mMeshes;
        std::vector<std::shared_ptr<Texture>> mAlbedoTextures;
        std::vector<std::shared_ptr<Texture>> mSpecularTextures;
        std::vector<std::shared_ptr<Texture>> mNormalTextures;
        std::vector<std::shared_ptr<Texture>> mHeightMapTextures;

    private:
        void loadModel(std::string const &path);
        void processNode(aiNode *node, const aiScene *scene);
        void processMesh(aiMesh *mesh, const aiScene *scene);
        void loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName);

        std::string mDirectory;
        std::set<std::string> mTexturesLoaded;
    };
}