// Based on
// https://learnopengl.com/code_viewer_gh.php?code=includes/learnopengl/mesh.h

#include "Core/mesh.hpp"

using namespace Core;


Mesh::Mesh(vector<Core::Vertex> vertices, vector<unsigned int> indices, vector<Core::Texture> textures) {
    this->vertices = vertices;
    this->indices = indices;
    this->textures = textures;

    // now that we have all the required data, set the vertex buffers and its attribute pointers.
    setupMesh();
}

Mesh::Mesh(vector<glm::vec3> positions, vector<glm::vec3> normals, vector<glm::vec2> texCoords,
           vector<Core::Texture> textures) {


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

void Mesh::draw(Core::Shader shader) {
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

void Mesh::setupMesh() {
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

void Mesh::center() {
    glm::vec3 totals(0);
    for (auto &vertex : vertices) {
        totals += vertex.Position;
    }
    glm::vec3 center = totals/glm::vec3(vertices.size());
    std::cout << "center is " << center[0] << ", " << center[1] << ", " << center[2] << std::endl;

    for (auto &vertex : vertices) {
        vertex.Position -= center;
        std::cout << "new position is " << vertex.Position[0] << ", " << vertex.Position[1] << ", " << vertex.Position[2] << std::endl;
    }

    setupMesh();
}