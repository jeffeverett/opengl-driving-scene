#pragma once

#include "Core/gameobject.hpp"
#include "Assets/shader.hpp"

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <bullet/btBulletDynamicsCommon.h>

#include <vector>

namespace Rendering
{
    struct Vertex
    {
        glm::vec3 position;
        glm::vec3 color;
    };

    class DebugRenderer : public btIDebugDraw
    {
    public:
        DebugRenderer();

        void drawAccumulated();
        void clear();

        virtual void drawLine(const btVector3 &from, const btVector3 &to, const btVector3 &color) override;
        virtual void drawContactPoint(const btVector3 &PointOnB, const btVector3 &normalOnB, btScalar distance, int lifeTime, const btVector3 &color);
        virtual void draw3dText (const btVector3 &location, const char *textString);
        virtual void reportErrorWarning(const char *warningString);
        virtual void setDebugMode(int debugMode) override;
        virtual int getDebugMode() const override;

    private:
        int mDebugMode;

        std::vector<Vertex> mVertices;
        
        std::unique_ptr<Assets::Shader> mShader;
        GLuint mVBO;
        GLuint mVAO;
    };

}
