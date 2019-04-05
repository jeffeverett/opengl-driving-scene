#pragma once

#include "Components/component.hpp"

#include <glm/glm.hpp>

namespace Components
{
    enum ProjectionMode {
        ORTHO,
        PERSPECTIVE
    };

    class Camera : public Component
    {
    public:
        Camera(const Core::GameObject &gameObject);
        virtual ~Camera();

        glm::mat4 getViewMatrix(glm::vec3 position, glm::vec3 target, glm::vec3 up);
        glm::mat4 getProjectionMatrix(float aspectRatio);
        void cycleProjectionMode();

        float mOrthoBoxSize = 10.0f;
        ProjectionMode mProjectionMode;
    };
}