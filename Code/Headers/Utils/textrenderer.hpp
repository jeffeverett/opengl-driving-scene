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
#include <map>

// Define Namespace
namespace Utils
{
    struct Character {
        GLuint     textureID;  // ID handle of the glyph texture
        glm::ivec2 size;       // Size of glyph
        glm::ivec2 bearing;    // Offset from baseline to left/top of glyph
        long     advance;    // Offset to advance to next glyph
    };

    class TextRenderer
    {
    public:

        // Implement Default Constructor and Destructor
        TextRenderer(std::string fontFile);
        ~TextRenderer() { }


        // Public Member Functions
        void resetVerticalOffset();
        void renderText(std::string text, GLfloat scale, glm::vec3 color = glm::vec3(0,0,0));

    private:

        // Disable Copying and Assignment
        TextRenderer(TextRenderer const &) = delete;
        TextRenderer & operator=(TextRenderer const &) = delete;

        // Private Member Variables
        GLuint mVBO;
        GLuint mVAO;
        std::shared_ptr<Utils::Shader> mShader;
        std::map<GLchar, Character> mCharacters;
        int mVerticalOffset;
    };
}
