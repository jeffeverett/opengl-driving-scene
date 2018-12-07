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

    void Scene::setFog(bool val) {
        defaultShader->use();
        if (val) {
            mFogEnabled = true;
            defaultShader->setFloat("fogDensity", 0.05f);
        }
        else {
            mFogEnabled = false;
            defaultShader->setFloat("fogDensity", 0.0f);
        }
    }

    void Scene::setNightMode(bool val) {
        defaultShader->use();
        if (val) {
            mNightModeEnabled = true;
            defaultShader->setVec3("dirLight.ambient", glm::vec3(0.1,0.1,0.1));
            defaultShader->setVec3("dirLight.diffuse", glm::vec3(0.25,0.25,0.25));
            defaultShader->setVec3("dirLight.specular", glm::vec3(1,1,1));
        }
        else {
            mNightModeEnabled = false;
            defaultShader->setVec3("dirLight.ambient", glm::vec3(0.3,0.3,0.3));
            defaultShader->setVec3("dirLight.diffuse", glm::vec3(0.95,0.95,0.95));
            defaultShader->setVec3("dirLight.specular", glm::vec3(1,1,1));
        }
    }

    void Scene::processInput(GLFWwindow *window) {
        // Window close
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, true);
        }

        // Night w/fog
        if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
            setFog(true);
            setNightMode(true);
        }

        // Night w/o fog
        if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS) {
            setFog(false);
            setNightMode(true);
        }

        // Day w/fog
        if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS) {
            setFog(true);
            setNightMode(false);
        }

        // Day w/o fog
        if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) {
            setFog(false);
            setNightMode(false);
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

        // Activate six streetlights closest to car
        float carZ = mCar->getPosition()[2];
        for (int i = 0; i < mStreetlights.size(); i+=2) {
            float streetlightZ = mStreetlights[i]->getPosition()[2];
            if (streetlightZ <= carZ) {
                // Streetlights immediately in front of car
                mStreetlights[i]->updateLighting(0);
                mStreetlights[i+1]->updateLighting(1);

                // Streetlights immediately behind car (if they exist)
                if (i-2 < 0) {
                    mStreetlights[i+2]->updateLighting(2);
                    mStreetlights[i+3]->updateLighting(3);
                    mStreetlights[i+4]->updateLighting(4);
                    mStreetlights[i+5]->updateLighting(5);

                }
                else {
                    mStreetlights[i-1]->updateLighting(2);
                    mStreetlights[i-2]->updateLighting(3);

                    // Next closest pair of streetlights
                    bool useForward;
                    if (i-4 < 0) {
                        useForward = true;
                    }
                    else if (i+3 >= mStreetlights.size()) {
                        useForward = false;
                    }
                    else {
                        float forwardStreetlightZ = mStreetlights[i+2]->getPosition()[2];
                        float backwardStreetlightZ = mStreetlights[i-3]->getPosition()[2];

                        useForward = abs(forwardStreetlightZ-carZ) <= abs(backwardStreetlightZ-carZ);
                    }

                    if (useForward) {
                        mStreetlights[i+2]->updateLighting(4);
                        mStreetlights[i+3]->updateLighting(5);
                    }
                    else {
                        mStreetlights[i-3]->updateLighting(4);
                        mStreetlights[i-4]->updateLighting(5);
                    }
                }
                break;
            }
        }
    }


    void Scene::add(std::shared_ptr<Utils::GameObject> gameObject) {
        mShaders.insert(gameObject->getShader());
        mGameObjects.push_back(gameObject);
    }

    void Scene::addStreetlight(std::shared_ptr<Objects::Streetlight> streetlight) {
        mStreetlights.push_back(streetlight);
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
