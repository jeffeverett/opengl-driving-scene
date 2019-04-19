#include "Rendering/debugrenderer.hpp"
#include "Utils/transformconversions.hpp"
#include "Utils/logger.hpp"

#define DEBUG
#include <iostream>

namespace Rendering
{
    DebugRenderer::DebugRenderer()
    {
        // Create shader
        mShader = std::make_unique<Assets::Shader>(
            PROJECT_SOURCE_DIR "/Shaders/VertexShaders/simple.vert",
            PROJECT_SOURCE_DIR "/Shaders/FragmentShaders/simple.frag"
        );

        // Create VAO/VBO
        glGenVertexArrays(1, &mVAO);
        glGenBuffers(1, &mVBO);
        glBindVertexArray(mVAO);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)sizeof(glm::vec3));

        // Set debugging mode
        setDebugMode(2);
    }

    void DebugRenderer::drawAccumulated()
    {
        mShader->use();
        glBindVertexArray(mVAO);
        glBindBuffer(GL_ARRAY_BUFFER, mVBO);
        glBufferData(GL_ARRAY_BUFFER, mVertices.size() * sizeof(Vertex), &mVertices[0], GL_STATIC_DRAW);
        glDrawArrays(GL_LINES, 0, mVertices.size());
    }

    void DebugRenderer::clear()
    {
        mVertices.clear();
    }

    void DebugRenderer::drawLine(const btVector3 &from, const btVector3 &to, const btVector3 &color)
    {
        #ifdef DEBUG
            Utils::Logger::log("from", from);
            Utils::Logger::log("to", to); 
        #endif

        Vertex fromVertex = {
            Utils::TransformConversions::btVector32glmVec3(from),
            Utils::TransformConversions::btVector32glmVec3(color)
        };
        Vertex toVertex = {
            Utils::TransformConversions::btVector32glmVec3(to),
            Utils::TransformConversions::btVector32glmVec3(color)
        };

        mVertices.push_back(fromVertex);
        mVertices.push_back(toVertex);
    }

    void DebugRenderer::drawContactPoint(const btVector3 &PointOnB, const btVector3 &normalOnB, btScalar distance, int lifeTime, const btVector3 &color)
    {
        //TODO
    }

    void DebugRenderer::draw3dText (const btVector3 &location, const char *textString)
    {
        //TODO
    }

    void DebugRenderer::reportErrorWarning(const char *warningString)
    {
        std::cerr << warningString << std::endl;
    }

    void DebugRenderer::setDebugMode(int debugMode)
    {
        mDebugMode = debugMode;
    }

    int DebugRenderer::getDebugMode() const
    {
        return mDebugMode;
    }
}