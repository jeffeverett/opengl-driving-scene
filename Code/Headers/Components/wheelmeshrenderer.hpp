#pragma once

#include "Components/component.hpp"
#include "Assets/material.hpp"
#include "Assets/mesh.hpp"

#include <glm/glm.hpp>

#include <vector>

namespace Components
{
    class WheelMeshRenderer : public Component
    {
    public:
        WheelMeshRenderer(Core::GameObject &gameObject);
        virtual ~WheelMeshRenderer();

        std::shared_ptr<Assets::Material> mMaterial;

        std::vector<std::shared_ptr<Assets::Mesh>> mWheelMeshes;
        std::vector<glm::mat4> mWheelModelMatrices;

    private:
        WheelMeshRenderer(WheelMeshRenderer const &) = delete;
        WheelMeshRenderer &operator=(WheelMeshRenderer const &) = delete;
    };
}
