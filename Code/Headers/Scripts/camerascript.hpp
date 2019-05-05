#pragma once

#include "Core/transform.hpp"
#include "Components/script.hpp"
#include "Components/camera.hpp"

namespace Scripts
{
    class CameraScript : public Components::Script
    {
    public:
        CameraScript(Core::GameObject &);
        virtual ~CameraScript();

        virtual void onStart(Core::Scene &scene) override;
        virtual void onUpdate(GLFWwindow *window, Core::Scene &scene, float deltaTime) override;

    private:
        float mRadius;
        float mTheta;
        float mPhi;

        std::shared_ptr<Components::Camera> mCamera;
        std::shared_ptr<Core::Transform> mCameraTransform;
    };
}
