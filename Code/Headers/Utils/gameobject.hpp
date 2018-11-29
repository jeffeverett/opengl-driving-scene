#pragma once

// Local Headers
#include "globals.hpp"
#include "Utils/drawable.hpp"
#include "Utils/shader.hpp"

// System Headers
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <btBulletDynamicsCommon.h>

// Standard Headers
#include <vector>
#include <memory>

// Define Namespace
namespace Utils
{
    class GameObject
    {
    public:

        // Implement Default Constructor and Destructor
        GameObject(std::shared_ptr<Utils::Drawable> drawable, std::shared_ptr<Utils::Shader> shader) {
            mDrawable = drawable;
            mShader = shader;
        }
        ~GameObject() {  }


        // Public Member Functions
        glm::vec3 getPosition();
        void setPosition(glm::vec3 position);
        float getTheta();
        void setOffset(glm::vec3 offset);
        void translate(glm::vec3 translationVector);
        void applyForce(glm::vec3 vec);
        void scale(glm::vec3 scaleVector);
        std::shared_ptr<Utils::Shader> getShader();
        void draw();
        virtual void perFrame(double deltaTime) {};
        virtual void processInput(GLFWwindow *window, double deltaTime) {}

    protected:
        std::unique_ptr<btRigidBody> mRigidBody;
        glm::vec3 mScale = glm::vec3(1);
        glm::vec3 mOffset = glm::vec3(0);
        float mTheta = 180;

        static glm::mat4 btScalar2glmMat4(btScalar* matrix);
        static glm::vec3 btVector32glmVec3(btVector3 matrix);
        static btVector3 glmVec32btVector3(glm::vec3 vec);

    private:

        // Disable Copying and Assignment
        GameObject(GameObject const &) = delete;
        GameObject & operator=(GameObject const &) = delete;

        // Private Member Variables
        std::shared_ptr<Utils::Drawable> mDrawable;
        std::shared_ptr<Utils::Shader> mShader;
    };
}
