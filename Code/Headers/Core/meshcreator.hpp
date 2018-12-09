#pragma once

// Local Headers
#include "Core/shader.hpp"
#include "Core/mesh.hpp"

// System Headers
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

// Standard Headers
#include <vector>
#include <memory>


// Define Namespace
namespace Core
{
    class MeshCreator
    {
    public:

        // Implement Default Constructor and Destructor
        MeshCreator() { }
        ~MeshCreator() { }


        // Public Member Functions
        std::shared_ptr<Core::Mesh> create();
        void addTexture(std::string filename, bool flipVertically = false);

        // Public Member Variables
        std::vector<glm::vec3> mPositions;
        std::vector<glm::vec3> mNormals;
        std::vector<glm::vec2> mTexCoords;
        std::vector<Core::Texture> mTextures;

    private:

        // Disable Copying and Assignment
        MeshCreator(MeshCreator const &) = delete;
        MeshCreator & operator=(MeshCreator const &) = delete;
    };
}