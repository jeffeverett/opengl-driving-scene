// https://learnopengl.com/code_viewer_gh.php?code=includes/learnopengl/model.h

#pragma once

#include <glad/glad.h>

#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <Core/drawable.hpp>
#include <Core/mesh.hpp>
#include <Core/shader.hpp>


#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>

using namespace std;
namespace Core
{
    unsigned int TextureFromFile(const char *path, const string &directory);

    class Model : public Drawable
    {
    public:
        /*  Model Data */
        vector<Texture> textures_loaded;	// stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
        vector<Mesh> meshes;
        string directory;
        bool gammaCorrection;

        /*  Functions   */
        // constructor, loads nothing
        Model() { }
        // constructor, expects a filepath to a 3D model.
        Model(string const &path, bool gamma = false);

        // draws the model, and thus all its meshes
        void draw(Shader shader) override;

    private:
        /*  Functions   */
        // loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
        void loadModel(string const &path);

        // processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
        void processNode(aiNode *node, const aiScene *scene);
        Mesh processMesh(aiMesh *mesh, const aiScene *scene);

        // checks all material textures of a given type and loads the textures if they're not loaded yet.
        // the required info is returned as a Texture struct.
        vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName);
    };
}