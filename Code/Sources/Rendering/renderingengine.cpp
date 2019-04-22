#include "Rendering/renderingengine.hpp"
#include "Rendering/rendersettings.hpp"
#include "Components/meshrenderer.hpp"
#include "Components/terrainrenderer.hpp"
#include "Components/meshfilter.hpp"
#include "Components/camera.hpp"
#include "Utils/openglerrors.hpp"
#include "Utils/transformconversions.hpp"
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
    // Enable/disable features that don't change
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 

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
    // Handle viewport changes
    glViewport(0, 0, scene.mRenderSettings.mFramebufferWidth, scene.mRenderSettings.mFramebufferHeight);
    if (scene.mRenderSettings.mFramebufferWidth != mTexWidth || scene.mRenderSettings.mFramebufferHeight != mTexHeight) {
      mTexWidth = scene.mRenderSettings.mFramebufferWidth;
      mTexHeight = scene.mRenderSettings.mFramebufferHeight;

      glBindTexture(GL_TEXTURE_2D, mGDepthID);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, mTexWidth, mTexHeight, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);

      glBindTexture(GL_TEXTURE_2D, mGPositionID);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, mTexWidth, mTexHeight, 0, GL_RGB, GL_FLOAT, NULL);

      glBindTexture(GL_TEXTURE_2D, mGNormalID);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, mTexWidth, mTexHeight, 0, GL_RGB, GL_FLOAT, NULL);
      
      glBindTexture(GL_TEXTURE_2D, mGAlbedoSpecID);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, mTexWidth, mTexHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    }

    // Calculate common uniforms
    calculateCameraUniforms(scene);

    // ***** GEOMETRY PASS *****
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);

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
      setCameraUniforms(material->mGeometryShader);
      setModelUniforms(material->mGeometryShader, scene, meshRenderer->mGameObject);
      
      // Draw
      auto meshFilters = meshRenderer->mGameObject.getComponents<Components::MeshFilter>();
      for (auto meshFilter : meshFilters) {
        mDrawCalls++;
        meshFilter->mMesh->draw();
      }
    }

    // Render terrains
    glBindVertexArray(mTerrainVAO);
    glPatchParameteri(GL_PATCH_VERTICES,4);
    auto terrainRenderers = scene.getComponents<Components::TerrainRenderer>();
    for (auto terrainRenderer : terrainRenderers) {
      // Prepare for draw
      auto material = terrainRenderer->mMaterial;
      prepareMaterialForRender(material);
      setTerrainUniforms(material->mGeometryShader, terrainRenderer);
      setCameraUniforms(material->mGeometryShader);
      setModelUniforms(material->mGeometryShader, scene, terrainRenderer->mGameObject);
      
      // Draw
      material->mGeometryShader->setFloat("heightScale", terrainRenderer->mHeightScale);
      material->mGeometryShader->setInt("gridSizeX", terrainRenderer->mPatchesX*terrainRenderer->mScaleX);
      material->mGeometryShader->setInt("gridSizeZ", terrainRenderer->mPatchesZ*terrainRenderer->mScaleZ);
      for (int i = -terrainRenderer->mPatchesZ/2; i < terrainRenderer->mPatchesZ/2; i++) {
        material->mGeometryShader->setInt("startZ", i*terrainRenderer->mScaleZ);
        for (int j = -terrainRenderer->mPatchesX/2; j < terrainRenderer->mPatchesX/2; j++) {     
          material->mGeometryShader->setInt("startX", j*terrainRenderer->mScaleX);
          mDrawCalls++;
          glDrawElements(GL_PATCHES, terrainN, GL_UNSIGNED_INT, 0);
        }
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
      // Draw main scene
      mLightingShader->use();
      mLightingShader->setInt("positionTexture", 0);
      mLightingShader->setInt("normalTexture", 1);
      mLightingShader->setInt("albedoSpecTexture", 2);
      mLightingShader->setVec2("scale", 1.0f, 1.0f);
      mLightingShader->setVec2("offset", 0.0f, 0.0f);
      setLightingUniforms(scene);
      drawQuad();

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

        mDebugRenderer->drawAccumulated();
      }
      mDebugRenderer->clear();
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
    mTextRenderer->renderText(fpsOSS.str(), 1, scene.mRenderSettings.mFramebufferWidth, scene.mRenderSettings.mFramebufferHeight, glm::vec3(0.5f, 0.5f, 0.5f));

    // Render draw calls as string
    std::ostringstream drawCallsOSS;
    drawCallsOSS << std::fixed << std::setprecision(5) << "Draw Calls: " << mDrawCalls;
    mTextRenderer->renderText(drawCallsOSS.str(), 1, scene.mRenderSettings.mFramebufferWidth, scene.mRenderSettings.mFramebufferHeight, glm::vec3(0.5f, 0.5f, 0.5f));
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
    auto cameraPos = camera->mGameObject.mTransform->mTranslation;
    mLightingShader->setVec3("viewPos", cameraPos);

    // Set directional light uniforms
    mLightingShader->setVec3("dirLight.direction", glm::vec3(0.3f, -0.7f, 0.648f));
    mLightingShader->setVec3("dirLight.ambient", glm::vec3(0.1f, 0.1f, 0.1f));
    mLightingShader->setVec3("dirLight.diffuse", glm::vec3(0.25f, 0.25f, 0.25f));
    mLightingShader->setVec3("dirLight.specular", glm::vec3(1.0f, 1.0f, 1.0f));

    // Set point light uniforms
    auto pointLights = scene.getComponents<Components::PointLight>();
    std::map<float, std::shared_ptr<Components::PointLight>> distancesMap;
    for (int i = 0; i < pointLights.size(); i++) {
      glm::vec3 pointLightPos = pointLights[i]->mGameObject.mTransform->mTranslation;
      float distance = glm::length(cameraPos-pointLightPos);
      distancesMap[distance] = pointLights[i];
    }
    auto iter = distancesMap.begin();
    for (int i = 0; i < std::min((size_t) 6, distancesMap.size()); i++) {
      std::string number = std::to_string(i);
      auto pointLight = iter->second;
      mLightingShader->setVec3("pointLights[" + number + "].position", pointLight->mGameObject.mTransform->mTranslation);
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

        mLightingShader->setVec3("spotLights[" + number + "].position", spotLight->mGameObject.mTransform->mTranslation);
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

  void RenderingEngine::setTerrainUniforms(std::shared_ptr<Assets::Shader> shader, std::shared_ptr<Components::TerrainRenderer> terrainRenderer)
  {
    shader->setFloat("innerTL", 32.0f);
    shader->setFloat("outerTL", 32.0f);
    shader->setFloat("scaleX", terrainRenderer->mScaleX);
    shader->setFloat("scaleZ", terrainRenderer->mScaleZ);
  }

}