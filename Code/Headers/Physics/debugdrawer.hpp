#pragma once

// Local Headers
#include "Core/gameobject.hpp"
#include "Core/textrenderer.hpp"
#include "Core/model.hpp"

// System Headers
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <bullet/btBulletDynamicsCommon.h>

// Standard Headers
#include <vector>
#include <memory>
#include <iomanip>

// Define Namespace
namespace Physics
{
    class DebugDrawer : public btIDebugDraw
    {
    public:
        DebugDrawer();

        virtual void drawLine(const btVector3 &from, const btVector3 &to, const btVector3 &color) override;
        virtual void drawContactPoint(const btVector3 &PointOnB, const btVector3 &normalOnB, btScalar distance, int lifeTime, const btVector3 &color);
        virtual void draw3dText (const btVector3 &location, const char *textString);
        virtual void reportErrorWarning(const char *warningString);
        virtual void setDebugMode(int debugMode) override;
        virtual int getDebugMode() const override;

    private:
        int mDebugMode;
        std::shared_ptr<Core::Shader> mShader;
    };

}
