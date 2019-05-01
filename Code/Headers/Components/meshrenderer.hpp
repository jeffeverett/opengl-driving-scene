#pragma once

#include "Components/component.hpp"
#include "Assets/material.hpp"

namespace Components
{
    class MeshRenderer : public Component
    {
    public:
        MeshRenderer(Core::GameObject &gameObject);
        virtual ~MeshRenderer();

        std::shared_ptr<Assets::Material> mMaterial;

    private:
        MeshRenderer(MeshRenderer const &) = delete;
        MeshRenderer &operator=(MeshRenderer const &) = delete;
    };
}
