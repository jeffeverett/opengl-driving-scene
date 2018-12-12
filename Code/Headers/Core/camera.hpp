#pragma once

// Local Headers
#include "Core/gameobject.hpp"


// System Headers
#include <glad/glad.h>
#include <glm/glm.hpp>

// Standard Headers
#include <vector>
#include <memory>

const float THETA = 0.0f;
const float PHI = 77.0f;
const float SENSITIVITY = 60.0f;
const float RADIUS = 9.0f;
const float BOX_SIZE = 25.0f;

namespace Core {
    // Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
    enum CameraMovement {
        FORWARD,
        BACKWARD,
        LEFT,
        RIGHT,
        UP,
        DOWN
    };

    enum ProjectionMode {
        ORTHO,
        PERSPECTIVE
    };

    // An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
    class Camera {
    public:
        // Constructor with vectors
        Camera(float radius = RADIUS, float theta = THETA, float phi = PHI) :
            mProjectionMode(PERSPECTIVE),
            mRadius(radius),
            mTheta(theta),
            mPhi(phi) {}

        glm::mat4 getViewMatrix();
        glm::mat4 getProjectionMatrix();
        void cycleProjectionMode();
        void perFrame();
        void processInput(GLFWwindow *window, double deltaTime);
        void setFollow(std::shared_ptr<Core::GameObject> follow);

    private:
        // Camera Attributes
        double mRadius;
        float mTheta;
        float mPhi;

        // Position is calculated once per frame and then stored here
        glm::vec3 mPosition;

        std::shared_ptr<Core::GameObject> mFollow;
        ProjectionMode mProjectionMode;
    };
}