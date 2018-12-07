#pragma once

#include "Core/shader.hpp"

namespace Core {
    class Drawable {
    public:
        virtual void draw(Shader shader) = 0;
    };
}