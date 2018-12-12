// Local Headers
#include "Core/camera.hpp"
#include "globals.hpp"

// System Headers
#include <glm/gtc/matrix_transform.hpp>

// Standard Headers
#include <vector>
#include <iostream>
#include <sstream>
#include <iomanip>


namespace Core {

    glm::mat4 Camera::getViewMatrix() {
        return glm::lookAt(
            mPosition,
            mFollow->getPosition(),
            glm::vec3(0, glm::cos(glm::radians(mPhi)), 0)
        );
    }

    glm::mat4 Camera::getProjectionMatrix() {
        if (mProjectionMode == ORTHO) {
            return glm::ortho(-BOX_SIZE, BOX_SIZE, -BOX_SIZE, BOX_SIZE, -BOX_SIZE, BOX_SIZE);
        }
        else {
            return glm::perspective(glm::radians(30.0f), ((float) width)/height, 0.1f, 300.0f);
        }
    }

    void Camera::cycleRelativeAngle() {
        mRelativeAngle = !mRelativeAngle;
    }

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
        textRenderer->renderText(ypOSS.str(), 1, defaultTextColor);

        std::ostringstream pmOSS;
        pmOSS << std::fixed << std::setprecision(5) << "Projection Mode: ";
        if (mProjectionMode == ORTHO) {
            pmOSS << "ORTHO";
        }
        else {
            pmOSS << "PERSPECTIVE";
        }
        textRenderer->renderText(pmOSS.str(), 1, defaultTextColor);

        glm::vec3 followForward = mFollow->getWorldForward();
        glm::vec3 projectedForward = glm::normalize(glm::vec3(followForward[0], 0, followForward[2]));
        double followTheta = glm::acos(projectedForward[2]);
        if (projectedForward[0] < 0) {
            followTheta = -followTheta;
        }
        double thetaRadians = glm::radians(mTheta);
        if (mRelativeAngle) {
            thetaRadians += followTheta + glm::radians(180.0f);
        }
        double phiRadians = glm::radians(mPhi);
        glm::vec3 offset(
            mRadius*glm::sin(thetaRadians)*glm::sin(phiRadians),
            mRadius*glm::cos(phiRadians),
            mRadius*glm::cos(thetaRadians)*sin(phiRadians)
        );

        mFollowFrames[mFollowIdx] = mFollow->getPosition();
        mFollowIdx = (mFollowIdx + 1) % FOLLOW_FRAMES;
        if (mFollowIdx == 0) {
            mFollowFramesFull = true;
        }

        glm::vec3 followSum(0);
        glm::vec3 followAvg;
        if (mFollowFramesFull) {
            for (int i = 0; i < FOLLOW_FRAMES; i++) {
                followSum += mFollowFrames[i];
            }
            followAvg = followSum*glm::vec3(1.0f/FOLLOW_FRAMES);
        }
        else {
            for (int i = 0; i < mFollowIdx; i++) {
                followSum += mFollowFrames[i];
            }
            followAvg = followSum*glm::vec3(1.0f/mFollowIdx);
        }

        mPosition = followAvg + offset;
        defaultShader->use();
        defaultShader->setVec3("viewPos", mPosition);

#ifdef DEBUG
        std::cout << "Camera Pos: " << mPosition[0] << ", " << mPosition[1] << ", " << mPosition[2] << ")" << std::endl;
#endif
    }

    void Camera::setFollow(std::shared_ptr<Core::GameObject> follow) {
        mFollow = follow;
    }

    void Camera::restoreDefaults() {
        mTheta = THETA;
        mPhi = PHI;
        mRadius = RADIUS;
    }

    void Camera::processInput(GLFWwindow *window, double deltaTime) {
        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
            mRadius += SENSITIVITY*deltaTime;
            if (mRadius > 100) {
                mRadius = 100;
            }
        }
        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
            mRadius -= SENSITIVITY*deltaTime;
            if (mRadius < 2) {
                mRadius = 2;
            }
        }
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
            mTheta -= SENSITIVITY*deltaTime;
        }
        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
            mTheta += SENSITIVITY*deltaTime;
        }
        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
            mPhi -= SENSITIVITY*deltaTime;
            if (mPhi < -85) {
                mPhi = -85;
            }
        }
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
            mPhi += SENSITIVITY*deltaTime;
            if (mPhi > 85) {
                mPhi = 85;
            }
        }
    }
}