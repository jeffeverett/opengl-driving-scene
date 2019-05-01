#include "Components/particlesystemrenderer.hpp"

namespace Components
{
  ParticleSystemRenderer::ParticleSystemRenderer(const Core::GameObject &gameObject) : Component(gameObject)
  {
  }

  ParticleSystemRenderer::~ParticleSystemRenderer()
  {
  }

  void ParticleSystemRenderer::update()
  {
    glBindVertexArray(mVAO);
    glDrawArrays(GL_POINTS, 0, mParticleIndices.size());
    glBindVertexArray(0);
   }

  void ParticleSystemRenderer::draw()
  {
    glBindVertexArray(mVAO);
    glDrawArrays(GL_POINTS, 0, mParticleIndices.size());
    glBindVertexArray(0);
   }

  void ParticleSystemRenderer::setupParticleSystem(unsigned int numParticles)
  {
    int n = ceil(sqrt(numParticles));
    mDataTextureWidth = n;
    mDataTextureHeight = n;

    // Create framebuffer
    glGenFramebuffers(1, &mBufferID);
    glBindFramebuffer(GL_FRAMEBUFFER, mBufferID);

    // Create data textures
    glGenTextures(1, &mPositionTimeTextureID);
    glBindTexture(GL_TEXTURE_2D, mPositionTimeTextureID);
    GLfloat texels[n][n] = {0};
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, n, n, 0, GL_RGBA, GL_FLOAT, texels);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mPositionTimeTextureID, 0);

    // Construct indices data
    for (int i = 0; i < n; i+=1) {
      for (int j = 0; j < n && i*n+j < numParticles; j+=1) {
        mParticleIndices.push_back(glm::ivec2(i,j));
      }
    }

    // Create buffers/arrays
    glGenVertexArrays(1, &mVAO);
    glGenBuffers(1, &mVBO);

    // Bind VAO/VBO and set buffer data
    glBindVertexArray(mVAO);
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glBufferData(GL_ARRAY_BUFFER, mParticleIndices.size() * sizeof(glm::ivec2), &mParticleIndices[0], GL_STATIC_DRAW);

    // Setup VAO
    // Particle indices
    glEnableVertexAttribArray(0);
    glVertexAttribIPointer(0, 2, GL_INT, sizeof(glm::ivec2), (void*)0);

    glBindVertexArray(0);
  }
}