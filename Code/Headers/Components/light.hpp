#pragma once

#include "Components/component.hpp"

namespace Components
{
    class Light : public Component
    {
    public:
        Light(const Core::GameObject &gameObject);
        virtual ~Light();

        glm::vec3 mAmbient;
        glm::vec3 mDiffuse;
        float mSpecular;
    private:
        Light(Light const &) = delete;
        Light &operator=(Light const &) = delete;
    };
}
