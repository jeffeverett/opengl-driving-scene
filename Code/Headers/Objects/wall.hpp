#pragma once

#include "Core/gameobject.hpp"
#include "Physics/physicsengine.hpp"
#include "Assets/mesh.hpp"
#include "Assets/material.hpp"

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <stb_image.h>

#include <vector>
#include <memory>

namespace Objects
{
    class Wall : public Core::GameObject
    {
    public:
        Wall(glm::vec3 position, const Physics::PhysicsEngine &physicsEngine);
        ~Wall() { }

        static void setup(std::shared_ptr<Assets::Shader> geometryShader, float trackInnerA, float trackInnerB, float trackOuterA, float trackOuterB);

    private:
        Wall(Wall const &) = delete;
        Wall & operator=(Wall const &) = delete;

        static std::shared_ptr<Assets::Material> mMaterial;
        static std::shared_ptr<Assets::Mesh> mMesh;
        static std::shared_ptr<btTriangleMesh> mColliderMesh;
    };
}
