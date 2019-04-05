#pragma once

#include "Components/light.hpp"
#include "Core/gameobject.hpp"

namespace Components
{
    class SpotLight : public Light
    {
    public:
        SpotLight(const Core::GameObject &gameObject);
        virtual ~SpotLight();

        float mInnerCutoff;
        float mOuterCutoff;

        float mConstant;
        float mLinear;
        float mQuadratic;

    private:
        SpotLight(SpotLight const &) = delete;
        SpotLight &operator=(SpotLight const &) = delete;
    };
}
