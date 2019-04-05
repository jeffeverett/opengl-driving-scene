#include "Rendering/physicsdebugrenderer.hpp"
#include "Utils/transformconversions.hpp"

#include <iostream>

namespace Rendering
{
    PhysicsDebugRenderer::PhysicsDebugRenderer()
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

    void PhysicsDebugRenderer::drawAccumulated()
    {
        mShader->use();
        glBindVertexArray(mVAO);
        glBindBuffer(GL_ARRAY_BUFFER, mVBO);
        glBufferData(GL_ARRAY_BUFFER, mVertices.size() * sizeof(Vertex), &mVertices[0], GL_STATIC_DRAW);
        glDrawArrays(GL_LINES, 0, mVertices.size());
    }

    void PhysicsDebugRenderer::clear()
    {
        mVertices.clear();
    }

    void PhysicsDebugRenderer::drawLine(const btVector3 &from, const btVector3 &to, const btVector3 &color)
    {
    #ifdef DEBUG
        //std::cout << "Drawing line with color (" << color[0] << ", " << color[1] << ", " << color[2] << ")" << std::endl;
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

    void PhysicsDebugRenderer::drawContactPoint(const btVector3 &PointOnB, const btVector3 &normalOnB, btScalar distance, int lifeTime, const btVector3 &color)
    {
        //TODO
    }

    void PhysicsDebugRenderer::draw3dText (const btVector3 &location, const char *textString)
    {
        //TODO
    }

    void PhysicsDebugRenderer::reportErrorWarning(const char *warningString)
    {
        std::cerr << warningString << std::endl;
    }

    void PhysicsDebugRenderer::setDebugMode(int debugMode)
    {
        mDebugMode = debugMode;
    }

    int PhysicsDebugRenderer::getDebugMode() const
    {
        return mDebugMode;
    }
}