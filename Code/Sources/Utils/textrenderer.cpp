// Local Headers
#include "globals.hpp"
#include "Utils/textrenderer.hpp"

// System Headers
#include <glm/gtc/matrix_transform.hpp>
#include <ft2build.h>
#include FT_FREETYPE_H

// Standard Headers
#include <iostream>

const GLfloat INITIAL_OFFSET_X = 10;
const GLfloat INITIAL_OFFSET_Y = 10;
const GLfloat PADDING_Y = 5;

namespace Utils
{
    void TextRenderer::resetVerticalOffset() {
        mVerticalOffset = 0;
    }

    TextRenderer::TextRenderer(std::string fontFile) : mVerticalOffset(0) {
        // Create textures
        FT_Library ft;
        if (FT_Init_FreeType(&ft))
            std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;

        FT_Face face;
        if (FT_New_Face(ft, fontFile.c_str(), 0, &face))
            std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;

        FT_Set_Pixel_Sizes(face, 0, 48);

        glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // Disable byte-alignment restriction

        for (GLubyte c = 0; c < 128; c++)
        {
            // Load character glyph
            if (FT_Load_Char(face, c, FT_LOAD_RENDER))
            {
                std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
                continue;
            }
            // Generate texture
            GLuint texture;
            glGenTextures(1, &texture);
            glBindTexture(GL_TEXTURE_2D, texture);
            glTexImage2D(
                GL_TEXTURE_2D,
                0,
                GL_RED,
                face->glyph->bitmap.width,
                face->glyph->bitmap.rows,
                0,
                GL_RED,
                GL_UNSIGNED_BYTE,
                face->glyph->bitmap.buffer
            );
            // Set texture options
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            // Now store character for later use
            Character character = {
                texture,
                glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
                glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
                face->glyph->advance.x
            };
            mCharacters.insert(std::pair<GLchar, Character>(c, character));
        }

        FT_Done_Face(face);
        FT_Done_FreeType(ft);

        // Create VAO/VBO
        glGenVertexArrays(1, &mVAO);
        glGenBuffers(1, &mVBO);
        glBindVertexArray(mVAO);
        glBindBuffer(GL_ARRAY_BUFFER, mVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, nullptr, GL_DYNAMIC_DRAW);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
        glEnableVertexAttribArray(0);

        // Create shader
        mShader = std::make_shared<Utils::Shader>(
            PROJECT_SOURCE_DIR "/Shaders/VertexShaders/text.vert",
            PROJECT_SOURCE_DIR "/Shaders/FragmentShaders/text.frag"
        );
        mShader->use();
        mShader->setInt("text", 0);

    }

    void TextRenderer::renderText(std::string text, GLfloat scale, glm::vec3 color) {
        // Activate corresponding render state
        mShader->use();
        mShader->setVec3("textColor", color);
        mShader->setMat4("projection", glm::ortho(0.0f, width, 0.0f, height));
        glActiveTexture(GL_TEXTURE0);
        glBindVertexArray(mVAO);

        // Iterate through all characters
        std::string::const_iterator c;
        GLfloat maxHeight = 0;
        GLfloat currentX = INITIAL_OFFSET_X;
        GLfloat currentY = INITIAL_OFFSET_Y;
        for (c = text.begin(); c != text.end(); c++)
        {
            Character ch = mCharacters[*c];

            GLfloat xpos = currentX + ch.bearing.x * scale;
            GLfloat ypos = currentY + mVerticalOffset - (ch.size.y - ch.bearing.y) * scale;

            GLfloat w = ch.size.x * scale;
            GLfloat h = ch.size.y * scale;

            if (h > maxHeight) {
                maxHeight = h;
            }

            // Update VBO for each character
            GLfloat vertices[6][4] = {
                { xpos,     ypos + h,   0.0, 0.0 },
                { xpos,     ypos,       0.0, 1.0 },
                { xpos + w, ypos,       1.0, 1.0 },

                { xpos,     ypos + h,   0.0, 0.0 },
                { xpos + w, ypos,       1.0, 1.0 },
                { xpos + w, ypos + h,   1.0, 0.0 }
            };

            // Render glyph texture over quad
            glBindTexture(GL_TEXTURE_2D, ch.textureID);
            // Update content of VBO memory
            glBindBuffer(GL_ARRAY_BUFFER, mVBO);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
            // Render quad
            glDrawArrays(GL_TRIANGLES, 0, 6);
            // Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
            currentX += (ch.advance >> 6) * scale; // Bitshift by 6 to get value in pixels (2^6 = 64)
        }

        mVerticalOffset += maxHeight + PADDING_Y;
    }
}