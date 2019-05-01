#pragma once

#include "Components/light.hpp"

namespace Components
{
    class DirectionalLight : public Light
    {
    public:
        DirectionalLight(Core::GameObject &gameObject);
        virtual ~DirectionalLight();
        
    private:
        DirectionalLight(DirectionalLight const &) = delete;
        DirectionalLight &operator=(DirectionalLight const &) = delete;
    };
}
