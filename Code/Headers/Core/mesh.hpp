// Based on
// https://learnopengl.com/code_viewer_gh.php?code=includes/learnopengl/mesh.h

#pragma once

#include <glad/glad.h> // holds all OpenGL type declarations

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <Core/drawable.hpp>
#include <Core/shader.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

using namespace std;

namespace Core
{
    struct Vertex {
        glm::vec3 Position;
        glm::vec3 Normal;
        glm::vec2 TexCoords;
        glm::vec3 Tangent;
        glm::vec3 Bitangent;
    };

    struct Texture {
        unsigned int id;
        string type;
        string path;
    };


    class Mesh : public Drawable {
    public:
        vector<Vertex> vertices;
        vector<unsigned int> indices;
        vector<Texture> textures;
        unsigned int VAO;

        Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures);
        Mesh(vector<glm::vec3> positions, vector<glm::vec3> normals, vector<glm::vec2> texCoords = vector<glm::vec2>(), vector<Texture> textures = vector<Texture>());

        void draw(Shader shader) override;
        void center();

    private:
        unsigned int VBO, EBO;

        void setupMesh();
    };
}