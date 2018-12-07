#pragma once

// Local Headers
#include "Core/camera.hpp"
#include "cubemap.hpp"
#include "Core/gameobject.hpp"
#include "Objects/car.hpp"
#include "Objects/streetlight.hpp"

// System Headers
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

// Standard Headers
#include <vector>
#include <memory>
#include <set>

// Define Namespace
namespace Core
{
    class Scene
    {
    public:

        // Implement Default Constructor and Destructor
        Scene() { }
        ~Scene() { }


        // Public Member Functions
        void add(std::shared_ptr<Core::GameObject> gameObject);
        void addStreetlight(std::shared_ptr<Objects::Streetlight> streetlight);
        void setCamera(std::shared_ptr<Core::Camera> camera);
        std::shared_ptr<Core::Camera> getCamera();
        void setFog(bool val);
        void setNightMode(bool val);
        void setCar(std::shared_ptr<Objects::Car> car);
        void setCubeMap(std::shared_ptr<Core::CubeMap> cubeMap);
        void setDeltaTime(double deltaTime);
        void updateLighting();
        void processInput(GLFWwindow *window);
        void draw();
        void perFrame();

    private:

        // Disable Copying and Assignment
        Scene(Scene const &) = delete;
        Scene & operator=(Scene const &) = delete;

        // Private Member Containers
        std::shared_ptr<Objects::Car> mCar;
        std::vector<std::shared_ptr<Objects::Streetlight>> mStreetlights;
        std::set<std::shared_ptr<Core::Shader>> mShaders;
        std::vector<std::shared_ptr<Core::GameObject>> mGameObjects;


        // Private Member Variables
        bool mFogEnabled;
        bool mNightModeEnabled;
        double mDeltaTime;
        std::shared_ptr<Core::CubeMap> mCubeMap;
        std::shared_ptr<Core::Camera> mCamera;
    };
}
