#pragma once

#define DEBUG

#include "Core/textrenderer.hpp"
#include "Core/shader.hpp"
#include "Physics/debugdrawer.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <btBulletDynamicsCommon.h>

#include <memory>

extern GLfloat width;
extern GLfloat height;

extern std::shared_ptr<Core::Shader> defaultShader;
extern std::shared_ptr<Core::Shader> simpleShader;
extern std::unique_ptr<Core::TextRenderer> textRenderer;
extern std::unique_ptr<btDiscreteDynamicsWorld> dynamicsWorld;
extern std::unique_ptr<Physics::DebugDrawer> debugDrawer;