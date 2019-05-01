#pragma once

#include "Components/script.hpp"
#include "Components/particlesystemrenderer.hpp"
#include "Components/carphysicsbody.hpp"
#include "Assets/particlesystem.hpp"

#include <glm/glm.hpp>

const int PSR_POOL_SIZE = 300;

namespace Scripts
{
    class CarScript : public Components::Script
    {
    public:
        CarScript(Core::GameObject &gameObject, glm::vec3 initialPosition);
        virtual ~CarScript();

        virtual void onStart(Core::Scene &scene) override;
        virtual void onUpdate(GLFWwindow *window, Core::Scene &scene, float deltaTime) override;

    private:
        glm::vec3 mInitialPosition;

        float mTimeSinceLastSpawnedPS;
        int mNextPoolIdx;

        std::shared_ptr<Components::CarPhysicsBody> mCarPhysicsBody;
        std::shared_ptr<Assets::ParticleSystem> mParticleSystem;

        std::shared_ptr<Components::ParticleSystemRenderer> mParticleSystemRendererPool[PSR_POOL_SIZE];
    };
}
