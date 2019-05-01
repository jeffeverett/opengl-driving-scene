#pragma once

#include "Assets/texture.hpp"
#include "Assets/shader.hpp"

#include <glm/glm.hpp>

#include <memory>
#include <vector>

namespace Assets
{
    class ParticleSystem
    {
    public:
        ParticleSystem();
        ~ParticleSystem();

        std::vector<std::shared_ptr<Texture>> mTextures;
        std::vector<glm::vec3> mColors;
        std::shared_ptr<Shader> mUpdateShader;
        std::shared_ptr<Shader> mRenderShader;

        float mParticleLifetime;
    };
}