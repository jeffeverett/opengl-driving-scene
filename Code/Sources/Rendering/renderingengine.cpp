#include "Rendering/renderingengine.hpp"
#include "Rendering/rendersettings.hpp"
#include "Components/meshrenderer.hpp"
#include "Components/meshfilter.hpp"
#include "Components/camera.hpp"
#include "Utils/openglerrors.hpp"
#include "globals.hpp"

#include <vector>
#include <iostream>
#include <iomanip>

float quadVertices[] = {
    -1.0f, 1.0f,
    -1.0f, -1.0f,
    1.0f, 1.0f,
    1.0f, -1.0f
};

namespace Rendering
{
  RenderingEngine::RenderingEngine()
  {
    // Enable/disable features that don't change
    glDisable(GL_BLEND);

    // Create auxiliary renderers
    mTextRenderer = std::make_unique<TextRenderer>(
      PROJECT_SOURCE_DIR "/Fonts/arial.ttf"
    );
    mPhysicsDebugRenderer = std::make_unique<PhysicsDebugRenderer>();

    // Set initial width/height of textures
    mTexWidth = INIT_WIDTH;
    mTexHeight = INIT_WIDTH;

    // Check errors
    if (glGetError()) {
      std::cout << "Error. Exiting." << std::endl;
      exit(1);
    }

    // Create gBuffer
    glGenFramebuffers(1, &mGBufferID);
    glBindFramebuffer(GL_FRAMEBUFFER, mGBufferID);

    // Create position color buffer
    glGenTextures(1, &mGPositionID);
    glBindTexture(GL_TEXTURE_2D, mGPositionID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, mTexWidth, mTexHeight, 0, GL_RGB, GL_FLOAT, NULL);
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

    // Create lighting shader
    mLightingShader = std::make_unique<Assets::Shader>(
      PROJECT_SOURCE_DIR "/Shaders/VertexShaders/quad.vert",
      PROJECT_SOURCE_DIR "/Shaders/FragmentShaders/lighting.frag"
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
  }

  RenderingEngine::~RenderingEngine()
  {
  }

  void RenderingEngine::drawQuad()
  {
    mDrawCalls++;
    glBindVertexArray(mQuadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
  }

  void RenderingEngine::renderScene(Core::Scene const & scene, double rollingFPS)
  {
    mDrawCalls = 0;

    // ***** SETUP *****
    glViewport(0, 0, scene.mRenderSettings.mScreenWidth, scene.mRenderSettings.mScreenHeight);
    if (scene.mRenderSettings.mScreenWidth != mTexWidth || scene.mRenderSettings.mScreenHeight != mTexHeight) {
      mTexWidth = scene.mRenderSettings.mScreenWidth;
      mTexHeight = scene.mRenderSettings.mScreenHeight;

      glBindTexture(GL_TEXTURE_2D, mGPositionID);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, mTexWidth, mTexHeight, 0, GL_RGB, GL_FLOAT, NULL);

      glBindTexture(GL_TEXTURE_2D, mGNormalID);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, mTexWidth, mTexHeight, 0, GL_RGB, GL_FLOAT, NULL);
      
      glBindTexture(GL_TEXTURE_2D, mGAlbedoSpecID);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, mTexWidth, mTexHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    }

    // ***** GEOMETRY PASS *****
    glEnable(GL_DEPTH_TEST);

    // Set framebuffer and clear
    glBindFramebuffer(GL_FRAMEBUFFER, mGBufferID);
    glClearColor(0.3f, 0.7f, 0.8f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Tell OpenGL which color attachments we'll use for rendering 
    unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
    glDrawBuffers(3, attachments);

    // Render each mesh attached to gameobjects with mesh renderers
    auto meshRenderers = scene.getComponents<Components::MeshRenderer>();
    for (auto meshRenderer : meshRenderers) {
      // Prepare for draw
      auto material = meshRenderer->mMaterial;
      prepareMaterialForRender(material);
      setCameraUniforms(material, scene);
      setModelUniforms(material, scene, meshRenderer->mGameObject);
      
      // Draw
      auto meshFilters = meshRenderer->mGameObject.getComponents<Components::MeshFilter>();
      for (auto meshFilter : meshFilters) {
        mDrawCalls++;
        meshFilter->mMesh->draw();
      }
    }

    // ***** SECOND PASS PREP *****
    // Set framebuffer and clear
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClearColor(0.3f, 0.7f, 0.8f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Make gBuffer information available
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, mGPositionID);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, mGNormalID);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, mGAlbedoSpecID);

    // ***** DEBUG PASS *****
    if (scene.mRenderSettings.mRenderMode == Rendering::RenderMode::DEBUG) {
      // Draw positions in top-left
      mDebugPositionShader->use();
      mDebugPositionShader->setInt("positionTexture", 0);
      mDebugPositionShader->setVec2("scale", 0.5f, 0.5f);
      mDebugPositionShader->setVec2("offset", -0.5f, 0.5f);
      drawQuad();

      // Draw normals in top-right
      mDebugNormalShader->use();
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
      // Draw main scene
      mLightingShader->use();
      mLightingShader->setInt("positionTexture", 0);
      mLightingShader->setInt("normalTexture", 1);
      mLightingShader->setInt("albedoSpecTexture", 2);
      mLightingShader->setVec2("scale", 1.0f, 1.0f);
      mLightingShader->setVec2("offset", 0.0f, 0.0f);
      drawQuad();

      // Draw physics debugging lines if enabled
      if (scene.mRenderSettings.mPhysicsDebug) {
        mPhysicsDebugRenderer->drawAccumulated();
      }
      mPhysicsDebugRenderer->clear();
    }


    // ***** RENDER UI *****
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glEnable(GL_DEPTH_TEST);

    // Start text renderer at bottom
    mTextRenderer->resetVerticalOffset();

    // Render FPS as string
    std::ostringstream fpsOSS;
    fpsOSS << std::fixed << std::setprecision(5) << "FPS: " << rollingFPS;
    mTextRenderer->renderText(fpsOSS.str(), 1, scene.mRenderSettings.mScreenWidth, scene.mRenderSettings.mScreenHeight, glm::vec3(0.5f, 0.5f, 0.5f));

    // Render draw calls as string
    std::ostringstream drawCallsOSS;
    drawCallsOSS << std::fixed << std::setprecision(5) << "Draw Calls: " << mDrawCalls;
    mTextRenderer->renderText(drawCallsOSS.str(), 1, scene.mRenderSettings.mScreenWidth, scene.mRenderSettings.mScreenHeight, glm::vec3(0.5f, 0.5f, 0.5f));
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
  }

  void RenderingEngine::setCameraUniforms(std::shared_ptr<Assets::Material> material, Core::Scene const &scene)
  {
    auto camera = scene.getComponents<Components::Camera>()[0];

    // Set projection matrix
    float aspectRatio = scene.mRenderSettings.mScreenWidth / scene.mRenderSettings.mScreenHeight;
    material->mGeometryShader->setMat4("projection", camera->getProjectionMatrix(aspectRatio));

    // Set view matrix
    glm::vec3 position = camera->mGameObject.mTransform->mTranslation;
    glm::vec3 target = glm::vec3(0);
    glm::vec3 up = glm::vec3(0, 1, 0);
    material->mGeometryShader->setMat4("view", camera->getViewMatrix(position, target, up));
  }

  void RenderingEngine::setModelUniforms(std::shared_ptr<Assets::Material> material, Core::Scene const &scene, const Core::GameObject &gameObject)
  {
    // Set model matrix
    material->mGeometryShader->setMat4("model", gameObject.mTransform->mModelMatrix);
  }
}