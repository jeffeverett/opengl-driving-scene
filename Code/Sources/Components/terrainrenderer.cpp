#include "Components/terrainrenderer.hpp"

namespace Components
{
  TerrainRenderer::TerrainRenderer(const Core::GameObject &gameObject) : Component(gameObject)
  {
  }

  TerrainRenderer::~TerrainRenderer()
  {
  }

  void TerrainRenderer::generateInstanceVBO()
  {
    mStartPositions.clear();
    for (int i = -mPatchesZ/2; i < mPatchesZ/2; i++) {
      float startZ = i*mScaleZ;
      for (int j = -mPatchesX/2; j < mPatchesX/2; j++) {
        float startX = j*mScaleX;
        mStartPositions.push_back(glm::vec2(startX, startZ));
      }
    }

    glGenBuffers(1, &mInstanceVBO);
    glBindBuffer(GL_ARRAY_BUFFER, mInstanceVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2)*mPatchesX*mPatchesZ, &mStartPositions[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0); 
  }
}