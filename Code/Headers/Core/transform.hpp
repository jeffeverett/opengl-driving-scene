#pragma once

#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

namespace Core
{
    class Transform
    {
    public:
        Transform();
        ~Transform();

        void scale(const glm::vec3 &scaleVector);
        void translate(const glm::vec3 &translationVector);
        void rotate(const glm::quat &quaternion);

        void updateModelMatrix(glm::mat4 startingMatrix);

        glm::vec3 mScale;
        glm::vec3 mTranslation;
        glm::quat mRotation;

        glm::mat4 mModelMatrix;

        bool mIsDirty;

    private:
        Transform(Transform const &) = delete;
        Transform &operator=(Transform const &) = delete;
    };
}
