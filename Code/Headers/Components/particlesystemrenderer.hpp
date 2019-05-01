#pragma once

#include "Components/component.hpp"
#include "Assets/particlesystem.hpp"

namespace Components
{
    class ParticleSystemRenderer : public Component
    {
    public:
        ParticleSystemRenderer(Core::GameObject &gameObject);
        virtual ~ParticleSystemRenderer();

        void setupParticleSystem(unsigned int numParticles);
        void resetBuffers();

        void update();
        void draw();

        std::shared_ptr<Assets::ParticleSystem> mParticleSystem;

        bool mIsActive;
        float mTimeActive;
        
        unsigned int mPositionBuffer;
        unsigned int mVelocityBuffer;
        unsigned int mColorBuffer;

        glm::vec3 mMaxOffset;


        int mNumParticles;

    private:
        ParticleSystemRenderer(ParticleSystemRenderer const &) = delete;
        ParticleSystemRenderer &operator=(ParticleSystemRenderer const &) = delete;

        unsigned int mVAO;
    };
}
