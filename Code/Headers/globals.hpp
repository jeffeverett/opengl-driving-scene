#pragma once

#include "Utils/textrenderer.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <memory>

extern GLfloat width;
extern GLfloat height;

extern std::unique_ptr<Utils::TextRenderer> textRenderer;