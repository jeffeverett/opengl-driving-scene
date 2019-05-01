#include "Rendering/renderingengine.hpp"
#include "Rendering/rendersettings.hpp"
#include "Components/meshrenderer.hpp"
#include "Components/wheelmeshrenderer.hpp"
#include "Components/terrainrenderer.hpp"
#include "Components/particlesystemrenderer.hpp"
#include "Components/meshfilter.hpp"
#include "Components/camera.hpp"
#include "Utils/openglerrors.hpp"
#include "Utils/transformconversions.hpp"
#include "Utils/logger.hpp"
#include "globals.hpp"

#include <vector>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <iomanip>
#include <Components/pointlight.hpp>
#include <Components/spotlight.hpp>

float quadVertices[] = {
  -1.0f, 1.0f,
  -1.0f, -1.0f,
  1.0f, 1.0f,
  1.0f, -1.0f
};

float terrainVertices[] = {
  0.0f, 0.0f,
  1.0f, 0.0f,
  1.0f, 1.0f,
  0.0f, 1.0f
};

int terrainIndices[] = {
  0,1,2,3
};

int terrainN = sizeof(terrainIndices)/sizeof(int);

namespace Rendering
{
  RenderingEngine::RenderingEngine(int texWidth, int texHeight) : mTexWidth(texWidth), mTexHeight(texHeight)
  {
    // OpenGL settings that don't change
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 
    glPointSize(1);

    // Create auxiliary renderers
    mTextRenderer = std::make_unique<TextRenderer>(
      PROJECT_SOURCE_DIR "/Fonts/arial.ttf"
    );
    mDebugRenderer = std::make_unique<DebugRenderer>();
    mDebugRenderer->setDebugMode(2);

    // Check errors
    if (glGetError()) {
      std::cout << "Error. Exiting." << std::endl;
      exit(1);
    }

    // Create gBuffer
    glGenFramebuffers(1, &mGBufferID);
    glBindFramebuffer(GL_FRAMEBUFFER, mGBufferID);

    // Create depth buffer
    glGenTextures(1, &mGDepthID);
    glBindTexture(GL_TEXTURE_2D, mGDepthID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, mTexWidth, mTexHeight, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, mGDepthID, 0);

    // Create position color buffer
    glGenTextures(1, &mGPositionID);
    glBindTexture(GL_TEXTURE_2D, mGPositionID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, mTexWidth, mTexHeight, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mGPositionID, 0);
      
    // Create normal color buffer
    glGenTextures(1, &mGNormalID);
    glBindTexture(GL_TEXTURE_2D, mGNormalID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, mTexWidth, mTexHeight, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, mGNormalID, 0);
      
    // Create color + specular color buffer
    glGenTextures(1, &mGAlbedoSpecID);
    glBindTexture(GL_TEXTURE_2D, mGAlbedoSpecID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, mTexWidth, mTexHeight, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, mGAlbedoSpecID, 0);
    
    // Check for completeness of framebuffer
    Utils::OpenGLErrors::checkFramebufferComplete();

    // Create lBuffer
    glGenFramebuffers(1, &mLBufferID);
    glBindFramebuffer(GL_FRAMEBUFFER, mLBufferID);
      
    // Create color buffer
    glGenTextures(1, &mLColorID);
    glBindTexture(GL_TEXTURE_2D, mLColorID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, mTexWidth, mTexHeight, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mLColorID, 0);
    
    // Check for completeness of framebuffer
    Utils::OpenGLErrors::checkFramebufferComplete();

    // Create lighting shader
    mLightingShader = std::make_unique<Assets::Shader>(
      PROJECT_SOURCE_DIR "/Shaders/VertexShaders/quad.vert",
      PROJECT_SOURCE_DIR "/Shaders/FragmentShaders/lighting.frag"
    );

    // Create FXAA shader
    mFXAAShader = std::make_unique<Assets::Shader>(
      PROJECT_SOURCE_DIR "/Shaders/VertexShaders/quad.vert",
      PROJECT_SOURCE_DIR "/Shaders/FragmentShaders/fxaa.frag"
    );

    // Create gBuffer debugging shaders
    mDebugPositionShader = std::make_unique<Assets::Shader>(
      PROJECT_SOURCE_DIR "/Shaders/VertexShaders/quad.vert",
      PROJECT_SOURCE_DIR "/Shaders/FragmentShaders/debug_position.frag"
    );
    mDebugNormalShader = std::make_unique<Assets::Shader>(
      PROJECT_SOURCE_DIR "/Shaders/VertexShaders/quad.vert",
      PROJECT_SOURCE_DIR "/Shaders/FragmentShaders/debug_normal.frag"
    );
    mDebugAlbedoShader = std::make_unique<Assets::Shader>(
      PROJECT_SOURCE_DIR "/Shaders/VertexShaders/quad.vert",
      PROJECT_SOURCE_DIR "/Shaders/FragmentShaders/debug_albedo.frag"
    );
    mDebugSpecShader = std::make_unique<Assets::Shader>(
      PROJECT_SOURCE_DIR "/Shaders/VertexShaders/quad.vert",
      PROJECT_SOURCE_DIR "/Shaders/FragmentShaders/debug_specular.frag"
    );

    // Create default textures
    mDefaultAlbedoTexture = std::make_unique<Assets::Texture>(
      PROJECT_SOURCE_DIR "/Textures/Defaults/default_albedo.jpg"
    );
    mDefaultNormalTexture = std::make_unique<Assets::Texture>(
      PROJECT_SOURCE_DIR "/Textures/Defaults/default_normal.jpg"
    );
    mDefaultSpecularTexture = std::make_unique<Assets::Texture>(
      PROJECT_SOURCE_DIR "/Textures/Defaults/default_specular.jpg"
    );

    // Create quad for framebuffer passes
    glGenVertexArrays(1, &mQuadVAO);
    glGenBuffers(1, &mQuadVBO);
    glBindVertexArray(mQuadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, mQuadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), (void*)0);
    glBindVertexArray(0);

    // Create quad for terrain rendering
    glGenVertexArrays(1, &mTerrainVAO);
    glBindVertexArray(mTerrainVAO);
    glGenBuffers(1, &mTerrainVBO);
    glBindBuffer(GL_ARRAY_BUFFER, mTerrainVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(terrainVertices), &terrainVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), (void*)0);
    glGenBuffers(1, &mTerrainEBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mTerrainEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(terrainIndices), &terrainIndices, GL_STATIC_DRAW);
  }

  RenderingEngine::~RenderingEngine()
  {
  }

  void RenderingEngine::clearFramebuffer()
  {
    glClearColor(0.3f, 0.7f, 0.8f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  }

  void RenderingEngine::drawQuad()
  {
    mDrawCalls++;
    glBindVertexArray(mQuadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
  }

  void RenderingEngine::renderScene(Core::Scene const & scene, double deltaTime, double rollingFPS)
  {
    mDrawCalls = 0;

    // ***** UPDATE PARTICLE STATES *****
    auto particleSystemRenderers = scene.getComponents<Components::ParticleSystemRenderer>();
    for (auto particleSystemRenderer : particleSystemRenderers) {
      if (particleSystemRenderer->mIsActive) {
        particleSystemRenderer->mTimeActive += deltaTime;

        auto updateShader = particleSystemRenderer->mParticleSystem->mUpdateShader;
        updateShader->use();

        // Set color uniforms
        auto colors = particleSystemRenderer->mParticleSystem->mColors;
        for (int i = 0; i < colors.size(); i++) {
          updateShader->setVec3("color" + std::to_string(i), colors[i]);
        }

        // Set particle update uniforms
        updateShader->setInt("numParticles", particleSystemRenderer->mNumParticles);
        updateShader->setFloat("particleLifetime", particleSystemRenderer->mParticleSystem->mParticleLifetime);
        updateShader->setFloat("totalTime", particleSystemRenderer->mTimeActive);
        updateShader->setFloat("deltaTime", deltaTime);

        setModelUniforms(updateShader, scene, particleSystemRenderer->mGameObject);
        particleSystemRenderer->update();
      }
    }

    // ***** MAIN RENDERING SETUP *****
    // Handle viewport changes
    glViewport(0, 0, scene.mRenderSettings.mFramebufferWidth, scene.mRenderSettings.mFramebufferHeight);
    if (scene.mRenderSettings.mFramebufferWidth != mTexWidth || scene.mRenderSettings.mFramebufferHeight != mTexHeight) {
      mTexWidth = scene.mRenderSettings.mFramebufferWidth;
      mTexHeight = scene.mRenderSettings.mFramebufferHeight;

      // Modify gBuffer textures
      glBindTexture(GL_TEXTURE_2D, mGDepthID);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, mTexWidth, mTexHeight, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);

      glBindTexture(GL_TEXTURE_2D, mGPositionID);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, mTexWidth, mTexHeight, 0, GL_RGBA, GL_FLOAT, NULL);

      glBindTexture(GL_TEXTURE_2D, mGNormalID);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, mTexWidth, mTexHeight, 0, GL_RGB, GL_FLOAT, NULL);
      
      glBindTexture(GL_TEXTURE_2D, mGAlbedoSpecID);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, mTexWidth, mTexHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

      // Modify lBuffer textures
      glBindTexture(GL_TEXTURE_2D, mLColorID);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, mTexWidth, mTexHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    }

    // Calculate common uniforms
    calculateCameraUniforms(scene);

    // ***** GEOMETRY PASS *****
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);

    // Set framebuffer and clear
    glBindFramebuffer(GL_FRAMEBUFFER, mGBufferID);
    clearFramebuffer();

    // Tell OpenGL which color attachments we'll use for rendering 
    unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
    glDrawBuffers(3, attachments);

    // Keep track of material&mesh combinations and their associated model matrices
    std::unordered_map<std::shared_ptr<Assets::Material>, std::unordered_map<std::shared_ptr<Assets::Mesh>, std::vector<glm::mat4>>> renderMap;

    // Prepare each mesh attached to gameobjects with mesh renderers
    auto meshRenderers = scene.getComponents<Components::MeshRenderer>();
    for (auto meshRenderer : meshRenderers) {
      auto gameObject = meshRenderer->mGameObject;
      auto material = meshRenderer->mMaterial;
      if (renderMap.find(material) == renderMap.end()) {
        renderMap[material] = std::unordered_map<std::shared_ptr<Assets::Mesh>, std::vector<glm::mat4>>();
      }

      auto meshFilters = meshRenderer->mGameObject.getComponents<Components::MeshFilter>();
      for (auto meshFilter : meshFilters) {
        auto mesh = meshFilter->mMesh;
        if (renderMap[material].find(mesh) == renderMap[material].end()) {
          renderMap[material][mesh] = std::vector<glm::mat4>();
        }
        renderMap[material][mesh].push_back(gameObject.mTransform->mModelMatrix);
      }
    }

    // Render prepared material&mesh combinations
    for (auto it = renderMap.begin(); it != renderMap.end(); it++) {
      auto material = it->first;
      prepareMaterialForRender(material);
      setCameraUniforms(material->mGeometryShader);
      for (auto innerIt = it->second.begin(); innerIt != it->second.end(); innerIt++) {
        auto mesh = innerIt->first;
        auto modelMatrices = innerIt->second;

        mDrawCalls++;
        mesh->drawInstanced(modelMatrices);
      }
    }
  
    // Render wheels
    auto wheelMeshRenderers = scene.getComponents<Components::WheelMeshRenderer>();
    for (auto wheelMeshRenderer : wheelMeshRenderers) {
      // Prepare for draw
      auto material = wheelMeshRenderer->mMaterial;
      prepareMaterialForRender(material);
      setCameraUniforms(material->mGeometryShader);
      
      // Prepare model matrices
      std::vector<glm::mat4> modelMatrices;
      for (int i = 0; i < wheelMeshRenderer->mWheelMeshes.size(); i++) {
        std::vector<glm::mat4> modelMatrices { wheelMeshRenderer->mWheelModelMatrices[i] };

        mDrawCalls++;
        wheelMeshRenderer->mWheelMeshes[i]->drawInstanced(modelMatrices);
      }
    }

    // Render terrains
    glPatchParameteri(GL_PATCH_VERTICES, 4);
    auto terrainRenderers = scene.getComponents<Components::TerrainRenderer>();
    for (auto terrainRenderer : terrainRenderers) {
      // Bind per-instance data to terrain VAO
      glBindVertexArray(mTerrainVAO);
      glBindBuffer(GL_ARRAY_BUFFER, terrainRenderer->mInstanceVBO);
      glEnableVertexAttribArray(1);
      glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)0);
      glVertexAttribDivisor(1, 1);  
      glEnableVertexAttribArray(2);
      glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)sizeof(float));
      glVertexAttribDivisor(2, 1);

      // Prepare for draw
      auto material = terrainRenderer->mMaterial;
      prepareMaterialForRender(material);
      setTerrainUniforms(material->mGeometryShader, scene, terrainRenderer);
      setCameraUniforms(material->mGeometryShader);
      setModelUniforms(material->mGeometryShader, scene, terrainRenderer->mGameObject);
      
      // Draw
      mDrawCalls++;
      glDrawElementsInstanced(GL_PATCHES, terrainN, GL_UNSIGNED_INT, 0, terrainRenderer->mPatchesX*terrainRenderer->mPatchesZ);
    }

    // ***** SECOND PASS PREP *****
    // Make gBuffer information available
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, mGPositionID);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, mGNormalID);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, mGAlbedoSpecID);

    // ***** DEBUG PASS *****
    if (scene.mRenderSettings.mRenderMode == Rendering::RenderMode::DEBUG) {
      // Do not use post-processing renders in deferred rendering debug
      glBindFramebuffer(GL_FRAMEBUFFER, 0);
      clearFramebuffer();

      // Draw positions in top-left
      mDebugPositionShader->use();
      mDebugPositionShader->setInt("positionTexture", 0);
      mDebugPositionShader->setVec2("scale", 0.5f, 0.5f);
      mDebugPositionShader->setVec2("offset", -0.5f, 0.5f);
      drawQuad();

      // Draw normals in top-right
      mDebugNormalShader->use();
      mDebugNormalShader->setMat4("view", mViewMtx);
      mDebugNormalShader->setInt("normalTexture", 1);
      mDebugNormalShader->setVec2("scale", 0.5f, 0.5f);
      mDebugNormalShader->setVec2("offset", 0.5f, 0.5f);
      drawQuad();

      // Draw albedo colors in bottom-left
      mDebugAlbedoShader->use();
      mDebugAlbedoShader->setInt("albedoSpecTexture", 2);
      mDebugAlbedoShader->setVec2("scale", 0.5f, 0.5f);
      mDebugAlbedoShader->setVec2("offset", -0.5f, -0.5f);
      drawQuad();

      // Draw specular intensities in bottom-right
      mDebugSpecShader->use();
      mDebugSpecShader->setInt("albedoSpecTexture", 2);
      mDebugSpecShader->setVec2("scale", 0.5f, 0.5f);
      mDebugSpecShader->setVec2("offset", 0.5f, -0.5f);
      drawQuad();
    }

    // ***** LIGHTING PASS *****
    if (scene.mRenderSettings.mRenderMode == Rendering::RenderMode::DEFERRED_SHADING) {
      // Render to lighting buffer if post-processing is necessary
      // Otherwise, render directly to default framebuffer
      if (scene.mRenderSettings.mFXAARenderMode != Rendering::FXAARenderMode::NONE) {
        glBindFramebuffer(GL_FRAMEBUFFER, mLBufferID);
      }
      else {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
      }
      clearFramebuffer();

      // Draw skybox
      scene.mCubeMap.mShader->use();
      setCameraUniforms(scene.mCubeMap.mShader);
      scene.mCubeMap.draw();

      // Draw main scene
      mLightingShader->use();
      mLightingShader->setInt("positionTexture", 0);
      mLightingShader->setInt("normalTexture", 1);
      mLightingShader->setInt("albedoSpecTexture", 2);
      mLightingShader->setVec2("scale", 1.0f, 1.0f);
      mLightingShader->setVec2("offset", 0.0f, 0.0f);
      setLightingUniforms(scene);
      drawQuad();

      // Use FXAA if enabled
      if (scene.mRenderSettings.mFXAARenderMode != Rendering::FXAARenderMode::NONE) {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        clearFramebuffer();

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, mLColorID);

        mFXAAShader->use();
        mFXAAShader->setInt("colorTexture", 0);
        mFXAAShader->setVec2("texelStep", glm::vec2(
          1.0f / scene.mRenderSettings.mFramebufferWidth, 1.0f / scene.mRenderSettings.mFramebufferHeight));
        mFXAAShader->setBool("showEdges", scene.mRenderSettings.mFXAARenderMode == Rendering::FXAARenderMode::FXAA_AND_DEBUG);
        mFXAAShader->setFloat("lumaThreshold", 0.5f);
        mFXAAShader->setFloat("mulReduce", 1.0f/8.0f);
        mFXAAShader->setFloat("minReduce", 1.0f/128.0f);
        mFXAAShader->setFloat("maxSpan", 8.0f);

        mFXAAShader->setVec2("scale", 1.0f, 1.0f);
        mFXAAShader->setVec2("offset", 0.0f, 0.0f);
        drawQuad();
      }

      // Draw physics debugging lines if enabled
      if (scene.mRenderSettings.mDrawDebugLines) {
        // Add debug lines for spot lights
        for (auto &spotLight : scene.getComponents<Components::SpotLight>()) {
          auto transform = spotLight->mGameObject.mTransform;
          mDebugRenderer->drawLine(
            Utils::TransformConversions::glmVec32btVector3(transform->getWorldTranslation()),
            Utils::TransformConversions::glmVec32btVector3(transform->getWorldTranslation() + spotLight->getDirection()),
            btVector3(1.0, 1.0, 1.0));
        }

        // Copy depth buffer from gBuffer to default framebuffer
        glBindFramebuffer(GL_READ_FRAMEBUFFER, mGBufferID);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // write to default framebuffer
        glBlitFramebuffer(
          0, 0, scene.mRenderSettings.mFramebufferWidth, scene.mRenderSettings.mFramebufferHeight,
          0, 0, scene.mRenderSettings.mFramebufferWidth, scene.mRenderSettings.mFramebufferHeight,
          GL_DEPTH_BUFFER_BIT, GL_NEAREST
        );
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // Set uniforms
        mDebugRenderer->mShader->use();
        setCameraUniforms(mDebugRenderer->mShader);

        // Draw
        mDebugRenderer->drawAccumulated();
      }
      mDebugRenderer->clear();

      // Render particles
      glEnable(GL_BLEND);
      glDepthMask(GL_FALSE);
      auto particleSystemRenderers = scene.getComponents<Components::ParticleSystemRenderer>();
      for (auto particleSystemRenderer : particleSystemRenderers) {
        if (particleSystemRenderer->mIsActive) {
          auto textures = particleSystemRenderer->mParticleSystem->mTextures;
          auto renderShader = particleSystemRenderer->mParticleSystem->mRenderShader;
          renderShader->use();
          renderShader->setVec2("particleSize", glm::vec2(0.2f, 0.2f));

          // Bind albedo textures
          for (int i = 0; i < textures.size(); i++) {
            glActiveTexture(GL_TEXTURE0 + i);
            renderShader->setInt("albedoMap" + std::to_string(i), i);
            glBindTexture(GL_TEXTURE_2D, textures[i]->mID);
          }
          
          setCameraUniforms(renderShader);
          particleSystemRenderer->draw();
        }
      }
    }


    // ***** RENDER UI *****
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glEnable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);

    // Start text renderer at bottom
    mTextRenderer->resetVerticalOffset();

    // Render FPS as string
    std::ostringstream fpsOSS;
    fpsOSS << std::fixed << std::setprecision(5) << "FPS: " << rollingFPS;
    mTextRenderer->renderText(fpsOSS.str(), 1, scene.mRenderSettings.mFramebufferWidth, scene.mRenderSettings.mFramebufferHeight, glm::vec3(1.0f, 1.0f, 1.0f));

    // Render draw calls as string
    std::ostringstream drawCallsOSS;
    drawCallsOSS << std::fixed << std::setprecision(5) << "Draw Calls: " << mDrawCalls;
    mTextRenderer->renderText(drawCallsOSS.str(), 1, scene.mRenderSettings.mFramebufferWidth, scene.mRenderSettings.mFramebufferHeight, glm::vec3(1.0f, 1.0f, 1.0f));
  }

  void RenderingEngine::calculateCameraUniforms(Core::Scene const &scene)
  {
    auto camera = scene.getComponents<Components::Camera>()[0];

    // Set projection matrix
    float aspectRatio = scene.mRenderSettings.mFramebufferWidth / scene.mRenderSettings.mFramebufferHeight;
    mProjectionMtx = camera->getProjectionMatrix(aspectRatio);

    // Set view matrix
    glm::vec3 up = glm::vec3(0, 1, 0);
    mViewMtx = camera->getViewMatrix(up);
  }

  void RenderingEngine::prepareMaterialForRender(std::shared_ptr<Assets::Material> material)
  {
    // Use geometry shader
    material->mGeometryShader->use();

    // Make textures available to shader
    glActiveTexture(GL_TEXTURE0);
    material->mGeometryShader->setInt("albedoMap", 0);
    if (material->mAlbedoMap) {
      glBindTexture(GL_TEXTURE_2D, material->mAlbedoMap->mID);
    }
    else {
      glBindTexture(GL_TEXTURE_2D, mDefaultAlbedoTexture->mID);
    }

    glActiveTexture(GL_TEXTURE1);
    material->mGeometryShader->setInt("normalMap", 1);
    if (material->mNormalMap) {
      glBindTexture(GL_TEXTURE_2D, material->mNormalMap->mID);
    }
    else {
      glBindTexture(GL_TEXTURE_2D, mDefaultNormalTexture->mID);
    }

    glActiveTexture(GL_TEXTURE2);
    material->mGeometryShader->setInt("specularMap", 2);
    if (material->mSpecularMap) {
      glBindTexture(GL_TEXTURE_2D, material->mSpecularMap->mID);
    }
    else {
      glBindTexture(GL_TEXTURE_2D, mDefaultSpecularTexture->mID);
    }

    glActiveTexture(GL_TEXTURE3);
    material->mGeometryShader->setInt("heightMap", 3);
    if (material->mHeightMap) {
      glBindTexture(GL_TEXTURE_2D, material->mHeightMap->mID);
    }
  }

  void RenderingEngine::setCameraUniforms(std::shared_ptr<Assets::Shader> shader)
  {
    shader->setMat4("projection", mProjectionMtx);
    shader->setMat4("view", mViewMtx);
  }

  void RenderingEngine::setLightingUniforms(Core::Scene const &scene)
  {
    // Set view position
    auto camera = scene.getComponents<Components::Camera>()[0];
    auto cameraPos = camera->getWorldTranslation();
    mLightingShader->setVec3("viewPos", cameraPos);

    // Set directional light uniforms
    mLightingShader->setVec3("dirLight.direction", glm::vec3(0.3f, -0.7f, 0.648f));
    mLightingShader->setVec3("dirLight.ambient", glm::vec3(0.1f, 0.1f, 0.1f));
    mLightingShader->setVec3("dirLight.diffuse", glm::vec3(0.25f, 0.25f, 0.25f));
    mLightingShader->setVec3("dirLight.specular", glm::vec3(0.5f, 0.5f, 0.5f));

    // Set point light uniforms
    auto followPos = camera->mFollowTransform->getWorldTranslation();
    auto pointLights = scene.getComponents<Components::PointLight>();
    std::map<float, std::shared_ptr<Components::PointLight>> distancesMap;
    for (int i = 0; i < pointLights.size(); i++) {
      glm::vec3 pointLightPos = pointLights[i]->mGameObject.mTransform->getWorldTranslation();
      float distance = glm::length(followPos-pointLightPos);
      distancesMap[distance] = pointLights[i];
    }
    auto iter = distancesMap.begin();
    for (int i = 0; i < std::min((size_t) 6, distancesMap.size()); i++) {
      std::string number = std::to_string(i);
      auto pointLight = iter->second;
      mLightingShader->setVec3("pointLights[" + number + "].position", pointLight->mGameObject.mTransform->getWorldTranslation());
      mLightingShader->setVec3("pointLights[" + number + "].ambient", pointLight->mAmbient);
      mLightingShader->setVec3("pointLights[" + number + "].diffuse", pointLight->mDiffuse);
      mLightingShader->setVec3("pointLights[" + number + "].specular", pointLight->mSpecular);
      mLightingShader->setFloat("pointLights[" + number + "].constant", pointLight->mConstant);
      mLightingShader->setFloat("pointLights[" + number + "].linear", pointLight->mLinear);
      mLightingShader->setFloat("pointLights[" + number + "].quadratic", pointLight->mQuadratic);
      iter++;
    }

    // Set spot light uniforms
    auto spotLights = scene.getComponents<Components::SpotLight>();
    for (int i = 0; i < spotLights.size(); i++) {
        std::string number = std::to_string(i);
        auto spotLight = spotLights[i];

        mLightingShader->setVec3("spotLights[" + number + "].position", spotLight->mGameObject.mTransform->getWorldTranslation());
        mLightingShader->setVec3("spotLights[" + number + "].direction", spotLight->getDirection());
        mLightingShader->setVec3("spotLights[" + number + "].ambient", spotLight->mAmbient);
        mLightingShader->setVec3("spotLights[" + number + "].diffuse", spotLight->mDiffuse);
        mLightingShader->setVec3("spotLights[" + number + "].specular", spotLight->mSpecular);
        mLightingShader->setFloat("spotLights[" + number + "].innerCutoff", spotLight->mInnerCutoff);
        mLightingShader->setFloat("spotLights[" + number + "].outerCutoff", spotLight->mOuterCutoff);
        mLightingShader->setFloat("spotLights[" + number + "].constant", spotLight->mConstant);
        mLightingShader->setFloat("spotLights[" + number + "].linear", spotLight->mLinear);
        mLightingShader->setFloat("spotLights[" + number + "].quadratic", spotLight->mQuadratic);
    }
  }

  void RenderingEngine::setModelUniforms(std::shared_ptr<Assets::Shader> shader, Core::Scene const &scene, const Core::GameObject &gameObject)
  {
    // Set model matrix
    shader->setMat4("model", gameObject.mTransform->mModelMatrix);
  }

  void RenderingEngine::setTerrainUniforms(std::shared_ptr<Assets::Shader> shader, Core::Scene const &scene, std::shared_ptr<Components::TerrainRenderer> terrainRenderer)
  {
    shader->setInt("wireframeMode", scene.mRenderSettings.mTerrainRenderMode);
    shader->setVec2("viewport", glm::vec2(scene.mRenderSettings.mFramebufferWidth, scene.mRenderSettings.mFramebufferHeight));
    shader->setFloat("scaleX", terrainRenderer->mScaleX);
    shader->setFloat("scaleZ", terrainRenderer->mScaleZ);
    shader->setFloat("heightScale", terrainRenderer->mHeightScale);
    shader->setInt("gridSizeX", terrainRenderer->mPatchesX*terrainRenderer->mScaleX);
    shader->setInt("gridSizeZ", terrainRenderer->mPatchesZ*terrainRenderer->mScaleZ);
    shader->setFloat("textureRepeatX", terrainRenderer->mTextureRepeatX);
    shader->setFloat("textureRepeatZ", terrainRenderer->mTextureRepeatZ);
  }

}