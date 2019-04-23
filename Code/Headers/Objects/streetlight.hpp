#pragma once

#include "Core/gameobject.hpp"
#include "Physics/physicsengine.hpp"
#include "Assets/mesh.hpp"
#include "Assets/material.hpp"

#include <glm/glm.hpp>

#include <memory>

namespace Objects
{
    class Streetlight : public Core::GameObject
    {
    public:
        Streetlight(glm::vec3 position, float theta, bool onLeft, const Physics::PhysicsEngine &physicsEngine);
        ~Streetlight();

        static void setup(std::shared_ptr<Assets::Shader> geometryShader);
    private:
        Streetlight(Streetlight const &) = delete;
        Streetlight & operator=(Streetlight const &) = delete;

        static std::shared_ptr<Assets::Mesh> mPostMesh;
        static std::shared_ptr<Assets::Material> mPostMaterial;

        static std::shared_ptr<Assets::Mesh> mBulbMesh;
        static std::shared_ptr<Assets::Material> mBulbMaterial;
    };
}
