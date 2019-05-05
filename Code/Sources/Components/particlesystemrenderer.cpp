#include "Components/particlesystemrenderer.hpp"

float frand(float min,float max)
{
   return rand()*(max-min)/RAND_MAX+min;
}


namespace Components
{
  ParticleSystemRenderer::ParticleSystemRenderer(Core::GameObject &gameObject) : Component(gameObject)
  {
  }

  ParticleSystemRenderer::~ParticleSystemRenderer()
  {
  }

  void ParticleSystemRenderer::update()
  {
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 6, mPositionBuffer);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 7, mVelocityBuffer);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 8, mColorBuffer);


    glDispatchCompute(mNumParticles, 1, 1);
    glMemoryBarrier(GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT);
  }

  void ParticleSystemRenderer::draw()
  {
    glBindVertexArray(mVAO);
    glDrawArrays(GL_POINTS, 0, mNumParticles);
    glBindVertexArray(0);
   }

  void ParticleSystemRenderer::setupParticleSystem(unsigned int numParticles)
  {
    mNumParticles = numParticles;

    // Create position buffer
    glGenBuffers(1, &mPositionBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, mPositionBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, mNumParticles*sizeof(glm::vec4), nullptr, GL_STATIC_DRAW);

    // Create velocity buffer
    glGenBuffers(1, &mVelocityBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, mVelocityBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, mNumParticles*sizeof(glm::vec4), nullptr, GL_STATIC_DRAW);

    // Create color buffer
    glGenBuffers(1, &mColorBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, mColorBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, mNumParticles*sizeof(glm::vec4), nullptr, GL_STATIC_DRAW);

    // Create VAO
    glGenVertexArrays(1, &mVAO);
    glBindVertexArray(mVAO);

    // Bind position buffer
    glBindBuffer(GL_ARRAY_BUFFER, mPositionBuffer);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), (void*)0);

    // Bind color buffer
    glBindBuffer(GL_ARRAY_BUFFER, mColorBuffer);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), (void*)0);

    resetBuffers();
  }

  void ParticleSystemRenderer::resetBuffers()
  {
    // Reset position buffer
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, mPositionBuffer);
    glm::vec4 *pos = (glm::vec4*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, mNumParticles*sizeof(glm::vec4), GL_MAP_WRITE_BIT|GL_MAP_INVALIDATE_BUFFER_BIT);
    for (int i = 0; i < mNumParticles; i++) {
      pos[i].x = frand(-mMaxOffset.x, mMaxOffset.x);
      pos[i].y = frand(-mMaxOffset.y, mMaxOffset.y);
      pos[i].z = frand(-mMaxOffset.z, mMaxOffset.z);
      pos[i].w = 0.0f;
    }
    glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

    // Reset velocity buffer
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, mVelocityBuffer);
    glm::vec4 *vel = (glm::vec4*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, mNumParticles*sizeof(glm::vec4), GL_MAP_WRITE_BIT|GL_MAP_INVALIDATE_BUFFER_BIT);
    for (int i = 0; i < mNumParticles; i++) {
      vel[i].x = 0.0f;
      vel[i].y = 0.0f;
      vel[i].z = 0.0f;
      vel[i].w = 0.0f;
    }
    glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

    // Reset color buffer
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, mColorBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, mNumParticles*sizeof(glm::vec4), nullptr, GL_STATIC_DRAW);
    glm::vec4 *col = (glm::vec4*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, mNumParticles*sizeof(glm::vec4), GL_MAP_WRITE_BIT|GL_MAP_INVALIDATE_BUFFER_BIT);
    for (int i = 0; i < mNumParticles; i++) {
      col[i].x = 0.0f;
      col[i].y = 0.0f;
      col[i].z = 0.0f;
      col[i].w = 0.0f;
    }
    glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
  }
}