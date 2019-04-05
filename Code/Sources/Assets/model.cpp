#include "Assets/model.hpp"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

#include <iostream>

const char pathSeparator =
#ifdef _WIN32
    '\\';
#else
    '/';
#endif

namespace Assets
{
    Model::Model(std::string const &path)
    {
        loadModel(path);
    }

    void Model::loadModel(std::string const &path)
    {
        // Read file via ASSIMP
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
        
        // Check for errors
        if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        {
            std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
            return;
        }
        
        // Retrieve the directory path of the filepath
        mDirectory = path.substr(0, path.find_last_of('/'));

        // Process ASSIMP's root node recursively
        processNode(scene->mRootNode, scene);
    }

    void Model::processNode(aiNode *node, const aiScene *scene)
    {
        // Process each mesh located at the current node
        for(unsigned int i = 0; i < node->mNumMeshes; i++) {
            // The node object only contains indices to index the actual objects in the scene.
            // The scene contains all the data, node is just to keep stuff organized (like relations between nodes).
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            processMesh(mesh, scene);
        }
        // After we've processed all of the meshes (if any) we then recursively process each of the children nodes
        for(unsigned int i = 0; i < node->mNumChildren; i++) {
            processNode(node->mChildren[i], scene);
        }
    }

    void Model::processMesh(aiMesh *mesh, const aiScene *scene)
    {
        // Data to fill
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        std::vector<Texture> textures;

        // Walk through each of the mesh's vertices
        for(unsigned int i = 0; i < mesh->mNumVertices; i++) {
            Vertex vertex;
            glm::vec3 vector;
            // Positions
            vector.x = mesh->mVertices[i].x;
            vector.y = mesh->mVertices[i].y;
            vector.z = mesh->mVertices[i].z;
            vertex.Position = vector;
            // Normals
            vector.x = mesh->mNormals[i].x;
            vector.y = mesh->mNormals[i].y;
            vector.z = mesh->mNormals[i].z;
            vertex.Normal = vector;
            // Texture coordinates
            if(mesh->mTextureCoords[0])
            {
                glm::vec2 vec;
                // a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't
                // use models where a vertex can have multiple texture coordinates so we always take the first set (0).
                vec.x = mesh->mTextureCoords[0][i].x;
                vec.y = mesh->mTextureCoords[0][i].y;
                vertex.TexCoords = vec;
            }
            else
                vertex.TexCoords = glm::vec2(0.0f, 0.0f);
            // Tangent
            vector.x = mesh->mTangents[i].x;
            vector.y = mesh->mTangents[i].y;
            vector.z = mesh->mTangents[i].z;
            vertex.Tangent = vector;
            // Bitangent
            vector.x = mesh->mBitangents[i].x;
            vector.y = mesh->mBitangents[i].y;
            vector.z = mesh->mBitangents[i].z;
            vertex.Bitangent = vector;
            vertices.push_back(vertex);
        }

        // Now wak through each of the mesh's faces and retrieve the corresponding vertex indices.
        for(unsigned int i = 0; i < mesh->mNumFaces; i++)
        {
            aiFace face = mesh->mFaces[i];
            // Retrieve all indices of the face and store them in the indices vector
            for(unsigned int j = 0; j < face.mNumIndices; j++)
                indices.push_back(face.mIndices[j]);
        }

        // Process materials
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        // We assume a convention for sampler names in the shaders. Each diffuse texture should be named
        // as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER.
        // Same applies to other texture as the following list summarizes:
        // diffuse: texture_diffuseN
        // specular: texture_specularN
        // normal: texture_normalN

        // Load textures
        loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
        loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
        loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
        loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");

        // return a mesh object created from the extracted mesh data
        mMeshes.push_back(std::make_shared<Mesh>(vertices, indices));
    }

    void Model::loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName)
    {
        for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
            aiString str;
            mat->GetTexture(type, i, &str);
            // Check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
            if (mTexturesLoaded.find(str.C_Str()) == mTexturesLoaded.end()) {
                // If the texture hasn't been loaded already, load it
                std::string path = mDirectory + pathSeparator + str.C_Str();
                std::shared_ptr<Texture> texture = std::make_shared<Texture>(path);
                
                if (typeName == "texture_diffuse") {
                    mAlbedoTextures.push_back(texture);
                }
                else if (typeName == "texture_specular") {
                    mSpecularTextures.push_back(texture);
                }
                else if (typeName == "texture_normal") {
                    mNormalTextures.push_back(texture);
                }
                else if (typeName == "texture_heightmap") {
                    mHeightMapTextures.push_back(texture);
                }
                mTexturesLoaded.insert(str.C_Str());
            }
        }
    }
}