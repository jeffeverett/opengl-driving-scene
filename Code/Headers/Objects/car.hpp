#pragma once

#include "Core/gameobject.hpp"
#include "Physics/physicsengine.hpp"
#include "Assets/model.hpp"
#include "Assets/material.hpp"
#include "Assets/particlesystem.hpp"
#include "Utils/meshcreator.hpp"

#include <memory>

namespace Objects
{
    class Car : public Core::GameObject
    {
    public:
        Car(glm::vec3 position, const Physics::PhysicsEngine &physicsEngine);
        ~Car();

        static void setup(std::shared_ptr<Assets::Shader> geometryShader);
    private:
        Car(Car const &) = delete;
        Car & operator=(Car const &) = delete;

        static std::shared_ptr<Assets::Model> mModel;
        static std::shared_ptr<Assets::Material> mMaterial;
        static std::shared_ptr<Assets::ParticleSystem> mParticleSystem;
    };
}
