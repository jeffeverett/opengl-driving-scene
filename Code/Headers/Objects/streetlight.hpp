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
    class Streetlight : public Utils::GameObject
    {
    public:

        // Implement Default Constructor and Destructor
        Streetlight(float z, bool onLeft);
        ~Streetlight() { }


        // Public Member Functions
        static void setup();
        void updateLighting(int lightIdx);
    private:

        // Disable Copying and Assignment
        Streetlight(Streetlight const &) = delete;
        Streetlight & operator=(Streetlight const &) = delete;

        // Private Member functions
        static int createOpenCylinder(float height1, float height2, float radius);
        static void createRotatedOpenCylinder(float height1, float height2, float radius, float x1, float x2, float angle1, float angle2);
        static void createSphere(glm::vec3 offset, float radius);

        // Private members
        static glm::vec3 mPointlightOffset;

        static std::shared_ptr<Utils::Drawable> mDrawable;
        static std::shared_ptr<Utils::Shader> mShader;
        static std::vector<Utils::Texture> mTextures;

        static std::vector<glm::vec3> mPositions;
        static std::vector<glm::vec3> mNormals;
        static std::vector<glm::vec2> mTexCoords;
    };
}
