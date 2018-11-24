// Local Headers
#include "Utils/scene.hpp"

// Standard Headers
#include <iostream>

// Define Namespace
namespace Utils
{
    void Scene::setCamera(std::shared_ptr<Utils::Camera> camera) {
        mCamera = camera;
    }

    std::shared_ptr<Camera> Scene::getCamera() {
        return mCamera;
    }

    void Scene::setCubeMap(std::shared_ptr<Utils::CubeMap> cubeMap) {
        mCubeMap = cubeMap;
    }

    void Scene::setDeltaTime(double deltaTime) {
        mDeltaTime = deltaTime;
    }

    void Scene::processInput(GLFWwindow *window) {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, true);
        }

        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
            mCamera->processKeyboard(Utils::CameraMovement::DOWN, mDeltaTime);
        }
        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
            mCamera->processKeyboard(Utils::CameraMovement::UP, mDeltaTime);
        }
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            mCamera->processKeyboard(Utils::CameraMovement::FORWARD, mDeltaTime);
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            mCamera->processKeyboard(Utils::CameraMovement::BACKWARD, mDeltaTime);
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            mCamera->processKeyboard(Utils::CameraMovement::LEFT, mDeltaTime);
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            mCamera->processKeyboard(Utils::CameraMovement::RIGHT, mDeltaTime);
        }
    }


    void Scene::add(std::shared_ptr<Utils::GameObject> gameObject) {
        mShaders.insert(gameObject->getShader());
        mGameObjects.push_back(gameObject);
    }

    void Scene::draw() {
        std::cout << "Drawing frame." << std::endl;
        for (auto &shader : mShaders) {
            shader->use();
            shader->setMat4("view", mCamera->getViewMatrix());
            shader->setMat4("projection", mCamera->getProjectionMatrix());
        }

        mCubeMap->draw();
        for (auto &gameObject : mGameObjects) {
            gameObject->draw();
        }
    }
}
