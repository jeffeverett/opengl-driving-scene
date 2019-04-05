#pragma once

#include "Core/gameobject.hpp"

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <stb_image.h>

#include <vector>
#include <memory>


namespace Objects
{
    class CustomObject
    {
    public:
        CustomObject(glm::vec3 position);
        ~CustomObject() { }

        static void setup();
    private:
        CustomObject(CustomObject const &) = delete;
        CustomObject & operator=(CustomObject const &) = delete;

        std::shared_ptr<Core::GameObject> mGameObject;
    };
}
