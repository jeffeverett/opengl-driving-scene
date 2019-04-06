#include "Assets/mesh.hpp"

#include <glad/glad.h>

#include <iostream>

namespace Assets
{
    Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices)
    {
        mVertices = vertices;
        mIndices = indices;

        // Now that we have all the required data, set the vertex buffers and its attribute pointers.
        setupMesh();
    }

    Mesh::Mesh(std::vector<glm::vec3> positions, std::vector<glm::vec3> normals, std::vector<glm::vec2> texCoords)
    {
        // Check for same number of elements
        if (positions.size() != normals.size()) {
            std::cout << "All containers should have same number of elements." << std::endl;
            return;
        }

        if (texCoords.size() != normals.size()) {
            std::cout << "All containers should have same number of elements." << std::endl;
            return;
        }

        // Ensure normals are normalized
        for (auto &normal : normals) {
            normal = glm::normalize(normal);
        }

        // Calculate tangents and bitangents
        //TODO averaging of shared vertices
        std::vector<glm::vec3> tangents;
        std::vector<glm::vec3> bitangents;

        for (int i = 0; i < positions.size(); i += 3) {
            glm::vec3 edge1 = positions[i+1] - positions[i];
            glm::vec3 edge2 = positions[i+2] - positions[i];
            glm::vec2 deltaUV1 = texCoords[i+1] - texCoords[i];
            glm::vec2 deltaUV2 = texCoords[i+2] - texCoords[i]; 
            
            float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

            glm::vec3 tangent;
            tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
            tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
            tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
            tangent = glm::normalize(tangent);
            tangents.push_back(tangent);

            glm::vec3 bitangent;
            bitangent.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
            bitangent.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
            bitangent.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
            bitangent = glm::normalize(bitangent);  
            bitangents.push_back(bitangent);
        }

        // Create vertices
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        for (unsigned int i = 0; i < positions.size(); i++) {
            vertices.push_back(Vertex{positions[i], normals[i], texCoords[i], tangents[i], bitangents[i]});
            indices.push_back(i);
        }
        mVertices = vertices;
        mIndices = indices;

        // Now that we have all the required data, set the vertex buffers and its attribute pointers.
        setupMesh();
    }

    Mesh::~Mesh()
    {
    }

    void Mesh::draw()
    {
        glBindVertexArray(mVAO);
        glDrawElements(GL_TRIANGLES, mIndices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

    void Mesh::setupMesh()
    {
        // Create buffers/arrays
        glGenVertexArrays(1, &mVAO);
        glGenBuffers(1, &mVBO);
        glGenBuffers(1, &mEBO);

        // Bind VAO/VBO/EBO and set buffer data
        glBindVertexArray(mVAO);
        glBindBuffer(GL_ARRAY_BUFFER, mVBO);
        glBufferData(GL_ARRAY_BUFFER, mVertices.size() * sizeof(Vertex), &mVertices[0], GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, mIndices.size() * sizeof(unsigned int), &mIndices[0], GL_STATIC_DRAW);

        // Setup VAO
        // Vertex Positions
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
        // Vertex normals
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
        // Vertex texture coords
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
        // Vertex tangent
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
        // Vertex bitangent
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));

        glBindVertexArray(0);
    }

    void Mesh::center()
    {
        glm::vec3 totals(0);
        for (auto &vertex : mVertices) {
            totals += vertex.Position;
        }
        glm::vec3 center = totals/glm::vec3(mVertices.size());

        for (auto &vertex : mVertices) {
            vertex.Position -= center;
        }

        setupMesh();
    }
}