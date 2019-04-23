#pragma once

#include <glm/glm.hpp>

#include <vector>

namespace Assets
{
    struct Vertex {
        glm::vec3 Position;
        glm::vec3 Normal;
        glm::vec2 TexCoords;
        glm::vec3 Tangent;
        glm::vec3 Bitangent;
    };

    class Mesh
    {
    public:
        Mesh();
        Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices);
        Mesh(std::vector<glm::vec3> positions, std::vector<glm::vec3> normals, std::vector<glm::vec2> texCoords);
        ~Mesh();

        void draw();
        void drawInstanced(std::vector<glm::mat4> modelMatrices);
        void center();
        void setupMesh();

        std::vector<Vertex> mVertices;
        std::vector<unsigned int> mIndices;

        unsigned int mVBO, mInstanceVBO, mEBO, mVAO;
    };
}