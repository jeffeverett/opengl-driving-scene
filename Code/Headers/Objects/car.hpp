#pragma once

// Local Headers
#include "globals.hpp"
#include "Core/gameobject.hpp"
#include "Core/model.hpp"
#include "Core/mesh.hpp"
#include "Core/meshcreator.hpp"

// System Headers
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

// Standard Headers
#include <vector>
#include <memory>
#include <iomanip>


// Define Namespace
namespace Objects
{
    class Car : public Core::GameObject
    {
    public:

        // Implement Default Constructor and Destructor
        Car();
        ~Car();

        // Public Member Functions
        static void setup();
        virtual void draw() override;
        void applyEngineForce(double force);
        void setSteering(double steering);
        void updateLighting();
        void processInput(GLFWwindow *window, double deltaTime) override;
        void perFrame(double deltaTime) override;

    private:

        // Disable Copying and Assignment
        Car(Car const &) = delete;
        Car & operator=(Car const &) = delete;

        // Private members
        double mSteering;
        btRaycastVehicle *mVehicle;

        static void createWheel(Core::MeshCreator &meshCreator);

        static glm::vec3 mSpotlightOffset1;
        static glm::vec3 mSpotlightOffset2;
        static glm::vec3 mTaillightOffset1;
        static glm::vec3 mTaillightOffset2;
        static std::shared_ptr<Core::Mesh> mWheelMesh;
        static std::shared_ptr<Core::Drawable> mDrawable;
        static std::shared_ptr<Core::Shader> mShader;
    };
}
