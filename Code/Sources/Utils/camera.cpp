// Local Headers
#include "Utils/camera.hpp"
#include "globals.hpp"

// System Headers
#include <glm/gtc/matrix_transform.hpp>

// Standard Headers
#include <vector>
#include <iostream>
#include <sstream>
#include <iomanip>

namespace Utils {

    // Returns the view matrix calculated using Euler Angles and the LookAt Matrix
    glm::mat4 Camera::getViewMatrix() {
        return glm::lookAt(
            mPosition,
            mFollow->getPosition(),
            glm::vec3(0, glm::cos(glm::radians(mPhi)), 0)
        );
    }

    // Returns the projection matrix
    glm::mat4 Camera::getProjectionMatrix() {
        if (mProjectionMode == ORTHO) {
            return glm::ortho(-BOX_SIZE, BOX_SIZE, -BOX_SIZE, BOX_SIZE, -BOX_SIZE, BOX_SIZE);
        }
        else {
            return glm::perspective(glm::radians(30.0f), width/height, 0.1f, 100.0f);
        }
    }

    // Set projection mode
    void Camera::cycleProjectionMode() {
        if (mProjectionMode == ORTHO) {
            mProjectionMode = PERSPECTIVE;
        }
        else {
            mProjectionMode = ORTHO;
        }
    }

    void Camera::perFrame() {
        std::ostringstream ypOSS;
        ypOSS << std::fixed << std::setprecision(5) << "Theta: " << fmod(mTheta, 360) << ", Phi: " << fmod(mPhi, 360) << ", Radius: " << mRadius;
        textRenderer->renderText(ypOSS.str(), 1, glm::vec3(0.5,0.5,0.5));

        std::ostringstream pmOSS;
        pmOSS << std::fixed << std::setprecision(5) << "Projection Mode: ";
        if (mProjectionMode == ORTHO) {
            pmOSS << "ORTHO";
        }
        else {
            pmOSS << "PERSPECTIVE";
        }
        textRenderer->renderText(pmOSS.str(), 1, glm::vec3(0.5,0.5,0.5));

        double thetaRadians = glm::radians(mTheta-mFollow->getTheta());
        double phiRadians = glm::radians(mPhi);
        glm::vec3 offset(
            -mRadius*glm::sin(thetaRadians)*glm::cos(phiRadians),
            mRadius*glm::sin(phiRadians),
            mRadius*glm::cos(thetaRadians)*cos(phiRadians)
        );
        mPosition = mFollow->getPosition() + offset;
        defaultShader->use();
        defaultShader->setVec3("viewPos", mPosition);

#ifdef DEBUG
        std::cout << "Camera Pos: " << mPosition[0] << ", " << mPosition[1] << ", " << mPosition[2] << ")" << std::endl;
#endif
    }

    void Camera::setFollow(std::shared_ptr<Utils::GameObject> follow) {
        mFollow = follow;
    }

    void Camera::processInput(GLFWwindow *window, double deltaTime) {
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
            mTheta = THETA;
            mPhi = PHI;
            mRadius = RADIUS;
        }
        if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
            cycleProjectionMode();
        }
        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
            mRadius += SENSITIVITY*deltaTime;
            if (mRadius > 30) {
                mRadius = 30;
            }
        }
        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
            mRadius -= SENSITIVITY*deltaTime;
            if (mRadius < 2) {
                mRadius = 2;
            }
        }
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
            mTheta += SENSITIVITY*deltaTime;
        }
        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
            mTheta -= SENSITIVITY*deltaTime;
        }
        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
           mPhi += SENSITIVITY*deltaTime;
           if (mPhi > 175) {
               mPhi = 175;
           }
        }
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
            mPhi -= SENSITIVITY*deltaTime;
            if (mPhi < 5) {
                mPhi = 5;
            }
        }
    }
}