#pragma once

#include "Rendering/textrenderer.hpp"
#include "Rendering/debugrenderer.hpp"
#include "Assets/material.hpp"
#include "Components/terrainrenderer.hpp"
#include "Core/scene.hpp"

#include <glad/glad.h>

namespace Rendering
{
    class RenderingEngine
    {
    public:
        RenderingEngine(int texWidth, int texHeight);
        ~RenderingEngine();

        void renderScene(Core::Scene const &scene, double rollingFPS);

        std::unique_ptr<DebugRenderer> mDebugRenderer;
        
    private:
        RenderingEngine(RenderingEngine const &) = delete;
        RenderingEngine & operator=(RenderingEngine const &) = delete;

        void calculateCameraUniforms(Core::Scene const &scene);
        void prepareMaterialForRender(std::shared_ptr<Assets::Material> material);
       
        void setCameraUniforms(std::shared_ptr<Assets::Shader> shader);
        void setModelUniforms(std::shared_ptr<Assets::Shader> shader, Core::Scene const &scene, const Core::GameObject &gameObject);
        void setLightingUniforms(Core::Scene const &scene);
        void setTerrainUniforms(std::shared_ptr<Assets::Shader> shader, std::shared_ptr<Components::TerrainRenderer> terrainRenderer);

        void drawQuad();

        glm::mat4 mProjectionMtx;
        glm::mat4 mViewMtx;

        std::unique_ptr<TextRenderer> mTextRenderer;

        int mDrawCalls;

        GLuint mGBufferID;
        GLuint mGDepthID, mGPositionID, mGNormalID, mGAlbedoSpecID;

        float mTexWidth;
        float mTexHeight;

        std::unique_ptr<Assets::Shader> mLightingShader;
        std::unique_ptr<Assets::Shader> mDebugPositionShader;
        std::unique_ptr<Assets::Shader> mDebugNormalShader;
        std::unique_ptr<Assets::Shader> mDebugAlbedoShader;
        std::unique_ptr<Assets::Shader> mDebugSpecShader;

        std::unique_ptr<Assets::Texture> mDefaultAlbedoTexture;
        std::unique_ptr<Assets::Texture> mDefaultNormalTexture;
        std::unique_ptr<Assets::Texture> mDefaultSpecularTexture;

        unsigned int mQuadVAO, mQuadVBO;
        unsigned int mTerrainVAO, mTerrainVBO, mTerrainEBO;
    };
}
