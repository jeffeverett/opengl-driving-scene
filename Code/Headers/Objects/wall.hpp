#pragma once

#include "Core/gameobject.hpp"
#include "Assets/mesh.hpp"
#include "Assets/material.hpp"
#include "Utils/meshcreator.hpp"

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
        Wall();
        ~Wall() { }

        static void addSegment(Utils::MeshCreator &meshCreator, int segNumber, float a, float b, float theta1, float theta2);
        static void setup();

    private:
        Wall(Wall const &) = delete;
        Wall & operator=(Wall const &) = delete;

        static std::shared_ptr<Assets::Mesh> mMesh;
        static std::shared_ptr<Assets::Material> mMaterial;
    };
}
