#pragma once

#include "Components/component.hpp"

namespace Components
{
    class Script : public Component
    {
    public:
        Script(const Core::GameObject &gameObject);
        virtual ~Script();

        virtual void onStart() = 0;
        virtual void onUpdate() = 0;
    private:
        Script(Script const &) = delete;
        Script &operator=(Script const &) = delete;
    };
}
