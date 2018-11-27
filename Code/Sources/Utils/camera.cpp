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

    // Constructor with vectors
    Camera::Camera(glm::vec3 position,
            glm::vec3 up,
            float yaw,
            float pitch) :
            mFront(glm::vec3(0.0f, 0.0f, -1.0f)),
            mMovementSpeed (SPEED),
            mMouseSensitivity(SENSITIVITY),
            mZoom(ZOOM),
            mBoxSize(BOX_SIZE) {
        mPosition = position;
        mWorldUp = up;
        mYaw = yaw;
        mPitch = pitch;
        updateCameraVectors();
    }

    // Constructor with scalar values
    Camera::Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) :
            mFront(glm::vec3(0.0f, 0.0f, -1.0f)),
            mMovementSpeed(SPEED),
            mMouseSensitivity(SENSITIVITY),
            mZoom(ZOOM),
            mBoxSize(BOX_SIZE) {

        mPosition = glm::vec3(posX, posY, posZ);
        mWorldUp = glm::vec3(upX, upY, upZ);
        mYaw = yaw;
        mPitch = pitch;
        updateCameraVectors();
    }

    // Returns the view matrix calculated using Euler Angles and the LookAt Matrix
    glm::mat4 Camera::getViewMatrix() {
        return glm::lookAt(mPosition, mPosition + mFront, mUp);
        //return glm::mat4(1);
    }

    // Returns the projection matrix
    glm::mat4 Camera::getProjectionMatrix() {
        if (projectionMode == ORTHO) {
            return glm::ortho(-mBoxSize, mBoxSize, -mBoxSize, mBoxSize, -mBoxSize, mBoxSize);
        }
        else {
            return glm::perspective(glm::radians(30.0f), 1280.0f/800.0f, 0.1f, 100.0f);
        }
    }

    // Set projection mode
    void Camera::cycleProjectionMode() {
        if (projectionMode == ORTHO) {
            this->projectionMode = PERSPECTIVE;
        }
        else {
            this->projectionMode = ORTHO;
        }
    }

    // Processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
    void Camera::processKeyboard(CameraMovement direction, float deltaTime) {
        float velocity = mMovementSpeed * deltaTime;
        if (direction == FORWARD)
            mPosition += mFront * velocity;
        else if (direction == BACKWARD)
            mPosition -= mFront * velocity;
        else if (direction == LEFT)
            mPosition -= mRight * velocity;
        else if (direction == RIGHT)
            mPosition += mRight * velocity;
        else if (direction == DOWN)
            mPosition -= mUp * velocity;
        else if (direction == UP)
            mPosition += mUp * velocity;

        std::cout << "Camera Position: (" << mPosition.x << ", " << mPosition.y << ", " << mPosition.z << ")" << std::endl;
    }

    // Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
    void Camera::processMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch) {
        xoffset *= mMouseSensitivity;
        yoffset *= mMouseSensitivity;

        mYaw += xoffset;
        mPitch += yoffset;

        // Make sure that when pitch is out of bounds, screen doesn't get flipped
        if (constrainPitch) {
            if (mPitch > 89.0f)
                mPitch = 89.0f;
            if (mPitch < -89.0f)
                mPitch = -89.0f;
        }

        // Update mFront, mRight and mUp Vectors using the updated Euler angles
        updateCameraVectors();
    }

    // Processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
    void Camera::processMouseScroll(float yoffset) {
        if (mZoom >= 1.0f && mZoom <= 45.0f)
            mZoom -= yoffset;
        if (mZoom <= 1.0f)
            mZoom = 1.0f;
        if (mZoom >= 45.0f)
            mZoom = 45.0f;
    }

    // Calculates the front vector from the Camera's (updated) Euler Angles
    void Camera::updateCameraVectors() {
        // Calculate the new mFront vector
        glm::vec3 front;
        front.x = cos(glm::radians(mYaw)) * cos(glm::radians(mPitch));
        front.y = sin(glm::radians(mPitch));
        front.z = sin(glm::radians(mYaw)) * cos(glm::radians(mPitch));
        mFront = glm::normalize(front);
        // Also re-calculate the mRight and mUp vector
        mRight = glm::normalize(glm::cross(mFront, mWorldUp));
        // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
        mUp = glm::normalize(glm::cross(mRight, mFront));

        std::cout << "Camera Up: (" << mUp.x << ", " << mUp.y << ", " << mUp.z << ")" << std::endl;
        std::cout << "Camera Right: (" << mRight.x << ", " << mRight.y << ", " << mRight.z << ")" << std::endl;
        std::cout << "Camera Front: (" << mFront.x << ", " << mFront.y << ", " << mFront.z << ")" << std::endl;
    }

    void Camera::perFrame() {
        std::ostringstream ypOSS;
        ypOSS << std::setprecision(5) << "Yaw: " << mYaw << ", Pitch: " << mPitch;
        textRenderer->renderText(ypOSS.str(), 1, glm::vec3(0.5,0.5,0.5));

        std::ostringstream cpOSS;
        cpOSS << std::setprecision(5) << "Camera Pos: (" << mPosition[0] << ", " << mPosition[1] << ", " << mPosition[2] << ")";
        textRenderer->renderText(cpOSS.str(), 1, glm::vec3(0.5,0.5,0.5));

        std::ostringstream pmOSS;
        pmOSS << std::setprecision(5) << "Projection Mode: ";
        if (projectionMode == ORTHO) {
            pmOSS << "ORTHO";
        }
        else {
            pmOSS << "PERSPECTIVE";
        }
        textRenderer->renderText(pmOSS.str(), 1, glm::vec3(0.5,0.5,0.5));
    }
}