#pragma once

#include "Assets/shader.hpp"
#include "Assets/texture.hpp"

#include <memory>

namespace Assets
{
    class Material
    {
    public:
        Material();
        ~Material();

        void prepareForRender();

        std::shared_ptr<Texture> mAlbedoMap;
        std::shared_ptr<Texture> mSpecularMap;
        std::shared_ptr<Texture> mNormalMap;

        std::shared_ptr<Shader> mGeometryShader;
    };
}