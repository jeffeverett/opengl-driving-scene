#pragma once

#include "Components/component.hpp"
#include "Assets/particlesystem.hpp"

namespace Components
{
    class ParticleSystemRenderer : public Component
    {
    public:
        ParticleSystemRenderer(const Core::GameObject &gameObject);
        virtual ~ParticleSystemRenderer();

        void setupParticleSystem(unsigned int numParticles);

        void update();
        void draw();

        std::shared_ptr<Assets::ParticleSystem> mParticleSystem;

        bool mIsActive;
        
        GLuint mBufferID;
        unsigned int mPositionTimeTextureID;

        int mDataTextureWidth;
        int mDataTextureHeight;

    private:
        ParticleSystemRenderer(ParticleSystemRenderer const &) = delete;
        ParticleSystemRenderer &operator=(ParticleSystemRenderer const &) = delete;
        
        std::vector<glm::ivec2> mParticleIndices;

        unsigned int mVBO, mVAO;
    };
}
