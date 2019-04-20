#include "Scripts/camerascript.hpp"
#include "Utils/logger.hpp"
#include "globals.hpp"

#include <iostream>

const float DEFAULT_RADIUS = 9.0f;
const float DEFAULT_THETA = 0.0f;
const float DEFAULT_PHI = 77.0f;

const float RADIUS_RATE = 60.0f;
const float ROTATION_RATE = 60.0f;

namespace Scripts
{
    CameraScript::CameraScript(const Core::GameObject &gameObject) : Script(gameObject),
      mRadius(DEFAULT_RADIUS), mTheta(DEFAULT_THETA), mPhi(DEFAULT_PHI)
    {
    }

    CameraScript::~CameraScript()
    {
    }

    void CameraScript::onStart()
    {
        mCamera = mGameObject.getComponents<Components::Camera>()[0];
        mCameraTransform = mCamera->mGameObject.mTransform;
    }

    void CameraScript::onUpdate(GLFWwindow *window, float deltaTime)
    {
        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
            mRadius += RADIUS_RATE * deltaTime;
            if (mRadius > 100) mRadius = 100;
        }
        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
            mRadius -= RADIUS_RATE * deltaTime;
            if (mRadius < 2) mRadius = 2;
        }
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
            mTheta -= ROTATION_RATE * deltaTime;
        }
        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
            mTheta += ROTATION_RATE * deltaTime;
        }
        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
            mPhi -= ROTATION_RATE * deltaTime;
            if (mPhi < -85) mPhi = -85;
        }
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
            mPhi += ROTATION_RATE * deltaTime;
            if (mPhi > 85) mPhi = 85;
        }

        float thetaRadians = glm::radians(mTheta);
        float phiRadians = glm::radians(mPhi);
        mCameraTransform->setTranslation(glm::vec3(
          mRadius*glm::sin(thetaRadians)*glm::sin(phiRadians),
          mRadius*glm::cos(phiRadians),
          mRadius*glm::cos(thetaRadians)*sin(phiRadians)
        ));

        Utils::Logger::log("Camera translation", mCameraTransform->mTranslation);
        std::cout << "Radius: " << mRadius << ", Theta: " << mTheta << ", Phi: " << mPhi << std::endl;
    }
}
