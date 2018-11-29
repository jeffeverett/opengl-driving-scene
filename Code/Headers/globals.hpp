#pragma once

//#define DEBUG

#include "Utils/textrenderer.hpp"
#include "Utils/shader.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <btBulletDynamicsCommon.h>

#include <memory>

extern GLfloat width;
extern GLfloat height;

extern std::shared_ptr<Utils::Shader> defaultShader;
extern std::shared_ptr<Utils::Shader> simpleShader;
extern std::unique_ptr<Utils::TextRenderer> textRenderer;
extern std::unique_ptr<btDiscreteDynamicsWorld> dynamicsWorld;