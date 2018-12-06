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

    void Scene::setCar(std::shared_ptr<Objects::Car> car) {
        mCar = car;
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

        // Enable fog
        if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
            mFogEnabled = true;
            defaultShader->use();
            defaultShader->setFloat("fogDensity", 0.05f);
        }

        // Disable fog
        if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS) {
            mFogEnabled = false;
            defaultShader->use();
            defaultShader->setFloat("fogDensity", 0.0f);
        }

        // Camera movement
        mCamera->processInput(window, mDeltaTime);

        // Forward to tracked gameobjects
        for (auto &gameObject : mGameObjects) {
            gameObject->processInput(window, mDeltaTime);
        }
    }

    void Scene::updateLighting() {
        mCar->updateLighting();
    }


    void Scene::add(std::shared_ptr<Utils::GameObject> gameObject) {
        mShaders.insert(gameObject->getShader());
        mGameObjects.push_back(gameObject);
    }

    void Scene::draw() {
#ifdef DEBUG
        std::cout << "Num shaders tracked by scene " << mShaders.size() << std::endl;
#endif
        simpleShader->use();
        simpleShader->setMat4("projection", mCamera->getProjectionMatrix());
        simpleShader->setMat4("view", mCamera->getViewMatrix());


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

        if (!mFogEnabled) {
            mCubeMap->draw();
        }

        updateLighting();

        for (auto &gameObject : mGameObjects) {
            gameObject->draw();
        }
    }

    void Scene::perFrame() {
        for (auto &gameObject : mGameObjects) {
            gameObject->perFrame(mDeltaTime);
        }

        mCamera->perFrame();
    }
}
