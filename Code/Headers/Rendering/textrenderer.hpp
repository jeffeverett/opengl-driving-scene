#pragma once

#include "Assets/shader.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <vector>
#include <memory>
#include <map>

namespace Rendering
{
    struct Character
    {
        GLuint textureID;      // ID handle of the glyph texture
        glm::ivec2 size;       // Size of glyph
        glm::ivec2 bearing;    // Offset from baseline to left/top of glyph
        long advance;          // Offset to advance to next glyph
    };

    class TextRenderer
    {
    public:
        TextRenderer(std::string fontFile);
        ~TextRenderer();

        void resetVerticalOffset();
        void renderText(std::string text, GLfloat scale, float screenWidth, float screenHeight, glm::vec3 color = glm::vec3(0,0,0));

    private:
        TextRenderer(TextRenderer const &) = delete;
        TextRenderer & operator=(TextRenderer const &) = delete;

        std::unique_ptr<Assets::Shader> mShader;

        GLuint mVBO;
        GLuint mVAO;
        std::map<GLchar, Character> mCharacters;
        float mVerticalOffset;
    };
}
