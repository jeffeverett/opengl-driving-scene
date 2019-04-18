#pragma once

#include "Components/component.hpp"

#include <GLFW/glfw3.h>

namespace Components
{
    class Script : public Component
    {
    public:
        Script(const Core::GameObject &gameObject);
        virtual ~Script();

        virtual void onStart() = 0;
        virtual void onUpdate(GLFWwindow *window, float deltaTime) = 0;
    private:
        Script(Script const &) = delete;
        Script &operator=(Script const &) = delete;
    };
}
