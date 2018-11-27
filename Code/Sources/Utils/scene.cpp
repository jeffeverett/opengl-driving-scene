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
        mShaders.insert(cubeMap->getShader());
        mCubeMap = cubeMap;
    }

    void Scene::setDeltaTime(double deltaTime) {
        mDeltaTime = deltaTime;
    }

    void Scene::processInput(GLFWwindow *window) {
        // Window close
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, true);
        }

        // Camera movement
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

        // Camera settings
        if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
            mCamera->cycleProjectionMode();
        }
    }


    void Scene::add(std::shared_ptr<Utils::GameObject> gameObject) {
        mShaders.insert(gameObject->getShader());
        mGameObjects.push_back(gameObject);
    }

    void Scene::draw() {
        std::cout << "Num shaders tracked by scene " << mShaders.size() << std::endl;
        for (auto &shader : mShaders) {
            shader->use();
            shader->setMat4("projection", mCamera->getProjectionMatrix());

            if (shader->ID == mCubeMap->getShader()->ID) {
                glm::mat4 strippedTransformView = glm::mat4(glm::mat3(mCamera->getViewMatrix()));
                shader->setMat4("view", strippedTransformView);
            }
            else {
                shader->setMat4("view", mCamera->getViewMatrix());
            }
        }

        mCubeMap->draw();
        for (auto &gameObject : mGameObjects) {
            gameObject->draw();
        }
    }

    void Scene::perFrame() {
        mCamera->perFrame();
    }
}
