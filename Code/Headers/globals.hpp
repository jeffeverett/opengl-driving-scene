#pragma once

//#define DEBUG

#include "Core/textrenderer.hpp"
#include "Core/shader.hpp"
#include "Physics/debugdrawer.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <bullet/btBulletDynamicsCommon.h>

#include <memory>

extern GLsizei width;
extern GLsizei height;

extern GLfloat trackInnerA;
extern GLfloat trackInnerB;
extern GLfloat trackOuterA;
extern GLfloat trackOuterB;

extern std::shared_ptr<Core::Shader> defaultShader;
extern std::shared_ptr<Core::Shader> simpleShader;
extern std::unique_ptr<Core::TextRenderer> textRenderer;
extern glm::vec3 defaultTextColor;
extern std::unique_ptr<btDiscreteDynamicsWorld> dynamicsWorld;
extern std::unique_ptr<Physics::DebugDrawer> debugDrawer;