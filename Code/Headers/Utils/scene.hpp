#pragma once

// Local Headers
#include "Utils/camera.hpp"
#include "cubemap.hpp"
#include "Utils/gameobject.hpp"

// System Headers
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

// Standard Headers
#include <vector>
#include <memory>
#include <set>

// Define Namespace
namespace Utils
{
    class Scene
    {
    public:

        // Implement Default Constructor and Destructor
        Scene() { }
        ~Scene() { }


        // Public Member Functions
        void add(std::shared_ptr<Utils::GameObject> gameObject);
        void setCamera(std::shared_ptr<Utils::Camera> camera);
        std::shared_ptr<Utils::Camera> getCamera();
        void setCubeMap(std::shared_ptr<Utils::CubeMap> cubeMap);
        void setDeltaTime(double deltaTime);
        void processInput(GLFWwindow *window);
        void draw();
        void perFrame();

    private:

        // Disable Copying and Assignment
        Scene(Scene const &) = delete;
        Scene & operator=(Scene const &) = delete;

        // Private Member Containers
        std::set<std::shared_ptr<Utils::Shader>> mShaders;
        std::vector<std::shared_ptr<Utils::GameObject>> mGameObjects;

        // Private Member Variables
        std::string mSceneName;
        double mDeltaTime;
        std::shared_ptr<Utils::CubeMap> mCubeMap;
        std::shared_ptr<Utils::Camera> mCamera;
    };
}
