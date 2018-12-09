#pragma once

// Local Headers
#include "globals.hpp"
#include "Core/gameobject.hpp"
#include "Core/mesh.hpp"
#include "Core/meshcreator.hpp"

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
    class Streetlight : public Core::GameObject
    {
    public:

        // Implement Default Constructor and Destructor
        Streetlight(glm::vec3 position, float theta, bool onLeft);
        ~Streetlight() { }


        // Public Member Functions
        static void setup();
        void updateLighting(int lightIdx);
    private:

        // Disable Copying and Assignment
        Streetlight(Streetlight const &) = delete;
        Streetlight & operator=(Streetlight const &) = delete;

        // Private Member functions
        static int createOpenCylinder(Core::MeshCreator &meshCreator, float height1, float height2, float radius);
        static void createRotatedOpenCylinder(Core::MeshCreator &meshCreator, float height1, float height2, float radius, float x1, float x2, float angle1, float angle2);
        static void createSphere(Core::MeshCreator &meshCreator, float maxPhi, glm::vec3 offset, float radius, float rotation);

        // Private members
        static glm::vec3 mPointlightOffset;

        static std::shared_ptr<Core::Drawable> mDrawable;
        static std::shared_ptr<Core::Shader> mShader;
    };
}
