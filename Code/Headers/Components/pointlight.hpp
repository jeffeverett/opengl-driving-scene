#pragma once

#include "Components/light.hpp"

namespace Components
{
    class PointLight : public Light
    {
    public:
        PointLight(const Core::GameObject &gameObject);
        virtual ~PointLight();

        float mConstant;
        float mLinear;
        float mQuadratic;
    private:
        PointLight(PointLight const &) = delete;
        PointLight &operator=(PointLight const &) = delete;
    };
}
