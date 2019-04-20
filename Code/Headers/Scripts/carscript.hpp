#pragma once

#include "Components/script.hpp"

#include <Components/carphysicsbody.hpp>

namespace Scripts
{
    class CarScript : public Components::Script
    {
    public:
        CarScript(Core::GameObject const &);
        virtual ~CarScript();

        virtual void onStart() override;
        virtual void onUpdate(GLFWwindow *window, float deltaTime) override;

    private:
        std::shared_ptr<Components::CarPhysicsBody> mCarPhysicsBody;
    };
}
