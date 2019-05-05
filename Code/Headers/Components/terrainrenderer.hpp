#pragma once

#include "Components/component.hpp"
#include "Assets/material.hpp"

namespace Components
{
    class TerrainRenderer : public Component
    {
    public:
        TerrainRenderer(Core::GameObject &gameObject);
        virtual ~TerrainRenderer();

        void generateInstanceVBO();

        int mHeightScale;
        int mPatchesX, mPatchesZ;
        float mScaleX, mScaleZ;

        float mTextureRepeatX, mTextureRepeatZ;

        GLuint mInstanceVBO;

        std::shared_ptr<Assets::Material> mMaterial;

    private:
        std::vector<glm::vec2> mStartPositions;

        TerrainRenderer(TerrainRenderer const &) = delete;
        TerrainRenderer &operator=(TerrainRenderer const &) = delete;
    };
}
