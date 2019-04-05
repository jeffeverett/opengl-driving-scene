#pragma once

#include "Assets/shader.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <vector>
#include <memory>


namespace Rendering
{
    class CubeMap
    {
    public:
        CubeMap() {}
        CubeMap(std::vector<std::string> faces);
        ~CubeMap() { }

        void setFaces(std::vector<std::string> faces);
        std::shared_ptr<Assets::Shader> getShader();
        void draw();

    private:
        CubeMap(CubeMap const &) = delete;
        CubeMap & operator=(CubeMap const &) = delete;

        GLuint mVAO;
        GLuint mVBO;
        GLuint mTexture;
        std::shared_ptr<Assets::Shader> mShader;
    };
}