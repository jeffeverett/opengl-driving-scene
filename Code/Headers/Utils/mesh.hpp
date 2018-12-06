#pragma once

#include <glad/glad.h> // holds all OpenGL type declarations

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <Utils/drawable.hpp>
#include <Utils/shader.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

using namespace std;

namespace Utils
{
    struct Vertex {
        // position
        glm::vec3 Position;
        // normal
        glm::vec3 Normal;
        // texCoords
        glm::vec2 TexCoords;
        // tangent
        glm::vec3 Tangent;
        // bitangent
        glm::vec3 Bitangent;
    };

    struct Texture {
        unsigned int id;
        string type;
        string path;
    };


    class Mesh : public Drawable {
    public:
        /*  Mesh Data  */
        vector<Vertex> vertices;
        vector<unsigned int> indices;
        vector<Texture> textures;
        unsigned int VAO;

        /*  Functions  */
        // constructor
        Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures)
        {
            this->vertices = vertices;
            this->indices = indices;
            this->textures = textures;

            // now that we have all the required data, set the vertex buffers and its attribute pointers.
            setupMesh();
        }


        Mesh(vector<glm::vec3> positions, vector<glm::vec3> normals, vector<glm::vec2> texCoords = vector<glm::vec2>(), vector<Texture> textures = vector<Texture>())
        {
            if (positions.size() != normals.size()) {
                cout << "Positions and normals should have same number of elements." << std::endl;
                return;
            }

            if (texCoords.size() > 0 && texCoords.size() != normals.size()) {
                cout << "Positions, normals, and textures should have same number of elements." << std::endl;
                return;
            }

            vector<Vertex> vertices;
            vector<unsigned int> indices;
            for (unsigned int i = 0; i < positions.size(); i++) {
                if (texCoords.size() > 0) {
                    vertices.push_back(Vertex{positions[i], normals[i], texCoords[i]});
                }
                else {
                    vertices.push_back(Vertex{positions[i], normals[i]});
                }
                indices.push_back(i);
            }
            this->vertices = vertices;
            this->indices = indices;
            this->textures = textures;

            // now that we have all the required data, set the vertex buffers and its attribute pointers.
            setupMesh();
        }

        // render the mesh
        void draw(Shader shader) override
        {
            bool specMap = false;
            shader.use();

            // bind appropriate textures
            unsigned int diffuseNr  = 1;
            unsigned int specularNr = 1;
            unsigned int normalNr   = 1;
            unsigned int heightNr   = 1;
            for(unsigned int i = 0; i < textures.size(); i++)
            {
                glActiveTexture(GL_TEXTURE0 + i); // active proper texture unit before binding
                // retrieve texture number (the N in diffuse_textureN)
                string uniformName;
                string name = textures[i].type;
                if(name == "texture_diffuse")
                    uniformName = "material.diffuse";
                else if(name == "texture_specular") {
                    uniformName = "material.specular";
                    specMap = true;
                }
                else if(name == "texture_normal")
                    uniformName = "material.normal";
                else if(name == "texture_height")
                    uniformName = "material.height";

                // now set the sampler to the correct texture unit
                glUniform1i(glGetUniformLocation(shader.ID, uniformName.c_str()), i);
                // and finally bind the texture
                glBindTexture(GL_TEXTURE_2D, textures[i].id);
            }

            if (specMap) {
                shader.setFloat("specStrength", 1);
            }
            else {
                shader.setFloat("specStrength", 0);
            }

            // draw mesh
            glBindVertexArray(VAO);
            glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);

            // always good practice to set everything back to defaults once configured.
            glActiveTexture(GL_TEXTURE0);
        }

    private:
        /*  Render data  */
        unsigned int VBO, EBO;

        /*  Functions    */
        // initializes all the buffer objects/arrays
        void setupMesh()
        {
            // create buffers/arrays
            glGenVertexArrays(1, &VAO);
            glGenBuffers(1, &VBO);
            glGenBuffers(1, &EBO);

            glBindVertexArray(VAO);
            // load data into vertex buffers
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            // A great thing about structs is that their memory layout is sequential for all its items.
            // The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
            // again translates to 3/2 floats which translates to a byte array.
            glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

            // set the vertex attribute pointers
            // vertex Positions
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
            // vertex normals
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
            // vertex texture coords
            glEnableVertexAttribArray(2);
            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
            // vertex tangent
            glEnableVertexAttribArray(3);
            glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
            // vertex bitangent
            glEnableVertexAttribArray(4);
            glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));

            glBindVertexArray(0);
        }
    };
}