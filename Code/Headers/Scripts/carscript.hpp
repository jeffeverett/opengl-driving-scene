#pragma once

#include "Components/script.hpp"
#include "Components/particlesystemrenderer.hpp"
#include "Components/carphysicsbody.hpp"

#include <glm/glm.hpp>

namespace Scripts
{
    class CarScript : public Components::Script
    {
    public:
        CarScript(Core::GameObject const &, glm::vec3 initialPosition);
        virtual ~CarScript();

        virtual void onStart() override;
        virtual void onUpdate(GLFWwindow *window, float deltaTime) override;

    private:
        glm::vec3 mInitialPosition;

        std::shared_ptr<Components::CarPhysicsBody> mCarPhysicsBody;
        std::shared_ptr<Components::ParticleSystemRenderer> mParticleSystemRenderer;
    };
}
