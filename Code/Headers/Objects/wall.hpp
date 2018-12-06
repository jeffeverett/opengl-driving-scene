#pragma once

// Local Headers
#include "globals.hpp"
#include "Utils/gameobject.hpp"
#include "Utils/mesh.hpp"

// System Headers
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <stb_image.h>

// Standard Headers
#include <vector>
#include <memory>


// Define Namespace
namespace Objects
{
    class Wall : public Utils::GameObject
    {
    public:

        // Implement Default Constructor and Destructor
        Wall();
        ~Wall() { }


        // Public Member Functions
        static void setup();
    private:

        // Disable Copying and Assignment
        Wall(Wall const &) = delete;
        Wall & operator=(Wall const &) = delete;

        // Private members
        static std::shared_ptr<Utils::Drawable> mDrawable;
        static std::shared_ptr<Utils::Shader> mShader;
        static std::vector<Utils::Texture> mTextures;

        static std::vector<glm::vec3> mPositions;
        static std::vector<glm::vec3> mNormals;
        static std::vector<glm::vec2> mTexCoords;
    };
}
