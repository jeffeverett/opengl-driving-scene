#pragma once

// Local Headers
#include "Utils/shader.hpp"

// System Headers
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

// Standard Headers
#include <vector>
#include <memory>


// Define Namespace
namespace Utils
{
    class CubeMap
    {
    public:

        // Implement Default Constructor and Destructor
        CubeMap(std::vector<std::string> faces);
        ~CubeMap() { }


        // Public Member Functions
        std::shared_ptr<Utils::Shader> getShader();
        void draw();

    private:

        // Disable Copying and Assignment
        CubeMap(CubeMap const &) = delete;
        CubeMap & operator=(CubeMap const &) = delete;

        // Private Member Variables
        GLuint mVAO;
        GLuint mVBO;
        GLuint mTexture;
        std::shared_ptr<Utils::Shader> mShader;
    };
}