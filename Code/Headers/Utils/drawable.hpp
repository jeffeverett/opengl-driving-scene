#pragma once

#include "Utils/shader.hpp"

namespace Utils {
    class Drawable {
    public:
        virtual void draw(Shader shader) = 0;
    };
}