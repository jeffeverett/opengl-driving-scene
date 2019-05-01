#pragma once

#include "Components/component.hpp"
#include "Core/scene.hpp"

#include <GLFW/glfw3.h>

namespace Components
{
    class Script : public Component
    {
    public:
        Script(Core::GameObject &gameObject);
        virtual ~Script();

        virtual void onStart(Core::Scene &scene) = 0;
        virtual void onUpdate(GLFWwindow *window, Core::Scene &scene, float deltaTime) = 0;
    private:
        Script(Script const &) = delete;
        Script &operator=(Script const &) = delete;
    };
}
