#pragma once

// Local Headers
#include "Utils/drawable.hpp"
#include "Utils/shader.hpp"

// System Headers
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

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
        ~GameObject() { }


        // Public Member Functions
        glm::vec3 getPosition();

        void translate(glm::vec3 translationVector);
        void scale(glm::vec3 scaleVector);
        void rotate(float angle);
        std::shared_ptr<Utils::Shader> getShader();
        void draw();
        virtual void perFrame(double deltaTime) {};
        virtual void processInput(GLFWwindow *window, double deltaTime) {}

    protected:
        glm::vec3 mScale = glm::vec3(1);
        glm::vec3 mPosition = glm::vec3(0);
        float mTheta = 0;

    private:

        // Disable Copying and Assignment
        GameObject(GameObject const &) = delete;
        GameObject & operator=(GameObject const &) = delete;

        // Private Member Variables
        std::shared_ptr<Utils::Drawable> mDrawable;
        std::shared_ptr<Utils::Shader> mShader;
    };
}
