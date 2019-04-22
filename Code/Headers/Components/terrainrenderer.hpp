#pragma once

#include "Components/component.hpp"
#include "Assets/material.hpp"

namespace Components
{
    class TerrainRenderer : public Component
    {
    public:
        TerrainRenderer(const Core::GameObject &gameObject);
        virtual ~TerrainRenderer();

        int mHeightScale;
        int mPatchesX, mPatchesZ;
        float mScaleX, mScaleZ;

        std::shared_ptr<Assets::Material> mMaterial;

    private:
        TerrainRenderer(TerrainRenderer const &) = delete;
        TerrainRenderer &operator=(TerrainRenderer const &) = delete;
    };
}
