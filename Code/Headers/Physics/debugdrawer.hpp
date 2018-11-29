#pragma once

// Local Headers
#include "globals.hpp"
#include "Utils/gameobject.hpp"
#include "Utils/textrenderer.hpp"
#include "Utils/model.hpp"

// System Headers
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

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
        DebugDrawer() {
            mShader = simpleShader;
        }

        virtual void drawLine(const btVector3 &from, const btVector3 &to, const btVector3 &color) override {
#ifdef DEBUG
            std::cout << "Drawing line" << std::endl;
#endif

            // https://pybullet.org/Bullet/phpBB3/viewtopic.php?t=11517
            GLuint VAO, VBO;
            GLfloat points[12];

            points[0] = from.x();
            points[1] = from.y();
            points[2] = from.z();
            points[3] = color.x();
            points[4] = color.y();
            points[5] = color.z();

            points[6] = to.x();
            points[7] = to.y();
            points[8] = to.z();
            points[9] = color.x();
            points[10] = color.y();
            points[11] = color.z();

            glDeleteBuffers(1, &VBO);
            glDeleteVertexArrays(1, &VAO);
            glGenBuffers(1, &VBO);
            glGenVertexArrays(1, &VAO);
            glBindVertexArray(VAO);
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(points), &points, GL_STATIC_DRAW);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), 0);
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
            glBindVertexArray(0);

            glBindVertexArray(VAO);
            mShader->use();
            glDrawArrays(GL_LINES, 0, 2);
            glBindVertexArray(0);

            /*GLuint VAO, VBO;
            GLfloat positions[6] = { from[0], from[1], from[2], to[0], to[1], to[2] };

            glGenVertexArrays(1, &VAO);
            glGenBuffers(1, &VBO);
            glBindVertexArray(VAO);
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*6, &positions[0], GL_STATIC_DRAW);

            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat), (void*)0);

            mShader->use();
            glDrawArrays(GL_LINES, 0, 6);*/
        }

        virtual void drawContactPoint(const btVector3 &PointOnB, const btVector3 &normalOnB, btScalar distance, int lifeTime, const btVector3 &color) {
            //TODO
        }

        virtual void draw3dText (const btVector3 &location, const char *textString) {
            textRenderer->renderText(textString, 1, glm::vec3(0.5,0.5,0.5));
        }

        virtual void reportErrorWarning(const char *warningString) {
            std::cerr << warningString << std::endl;
        }

        virtual void setDebugMode(int debugMode) override {
            mDebugMode = debugMode;
        }

        virtual int getDebugMode() const override {
            return mDebugMode;
        }

    private:
        int mDebugMode;
        std::shared_ptr<Utils::Shader> mShader;
    };

}
