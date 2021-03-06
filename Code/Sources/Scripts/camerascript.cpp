#include "Scripts/camerascript.hpp"
#include "Utils/logger.hpp"
#include "globals.hpp"

#include <iostream>

const float SCALE_FACTOR = 1.0f;


const float DEFAULT_RADIUS = 9.0f;
const float DEFAULT_THETA = 0.0f;
const float DEFAULT_PHI = 77.0f;

const float RADIUS_RATE = 60.0f;
const float ROTATION_RATE = 60.0f;

namespace Scripts
{
    CameraScript::CameraScript(Core::GameObject &gameObject) : Script(gameObject),
      mRadius(DEFAULT_RADIUS), mTheta(DEFAULT_THETA), mPhi(DEFAULT_PHI)
    {
    }

    CameraScript::~CameraScript()
    {
    }

    void CameraScript::onStart(Core::Scene &scene)
    {
        mCamera = mGameObject.getComponents<Components::Camera>()[0];
        mCameraTransform = mCamera->mGameObject.mTransform;
    }

    void CameraScript::onUpdate(GLFWwindow *window, Core::Scene &scene, float deltaTime)
    {
        // Key for resetting camera
        if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
            mRadius = DEFAULT_RADIUS;
            mTheta = DEFAULT_THETA;
            mPhi = DEFAULT_PHI;
        }

        // Keys for modifying radius/theta/phi
        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
            mRadius += RADIUS_RATE * deltaTime;
            if (mRadius > 250) mRadius = 250;
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
          SCALE_FACTOR*mRadius*glm::sin(thetaRadians)*glm::sin(phiRadians),
          SCALE_FACTOR*mRadius*glm::cos(phiRadians),
          SCALE_FACTOR*mRadius*glm::cos(thetaRadians)*sin(phiRadians)
        ));

        #ifdef DEBUG
            Utils::Logger::log("Camera translation", mCameraTransform->mTranslation);
            Utils::Logger::log("Camera world translation", mCameraTransform->getWorldTranslation());
            Utils::Logger::log("Camera model matrix", mCameraTransform->mModelMatrix);

            std::cout << "Radius: " << mRadius << ", Theta: " << mTheta << ", Phi: " << mPhi << std::endl;
        #endif
    }
}
