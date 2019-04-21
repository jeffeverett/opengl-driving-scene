#include "Components/camera.hpp"

#include <glm/gtc/matrix_transform.hpp>

namespace Components
{
    Camera::Camera(const Core::GameObject &gameObject) : Component(gameObject)
    {
    }

    Camera::~Camera()
    {
    }

    glm::mat4 Camera::getViewMatrix(glm::vec3 up)
    {
        return glm::lookAt(
            mGameObject.mTransform->getWorldTranslation(),
            mFollowTransform->getWorldTranslation(),
            up
        );
    }

    glm::mat4 Camera::getProjectionMatrix(float aspectRatio)
    {
        if (mProjectionMode == ORTHO) {
            return glm::ortho(-mOrthoBoxSize, mOrthoBoxSize, -mOrthoBoxSize, mOrthoBoxSize, -mOrthoBoxSize, mOrthoBoxSize);
        }
        else {
            return glm::perspective(glm::radians(30.0f), aspectRatio, 0.1f, 300.0f);
        }
    }

    void Camera::cycleProjectionMode()
    {
        if (mProjectionMode == ORTHO) {
            mProjectionMode = PERSPECTIVE;
        }
        else {
            mProjectionMode = ORTHO;
        }
    }
}