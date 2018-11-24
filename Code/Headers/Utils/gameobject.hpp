#pragma once

// Local Headers
#include "Utils/model.hpp"
#include "Utils/shader.hpp"

// System Headers
#include <glad/glad.h>
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
        GameObject(std::shared_ptr<Utils::Model> model, std::shared_ptr<Utils::Shader> shader) {
            mModel = model;
            mShader = shader;
        }
        ~GameObject() { }


        // Public Member Functions
        void translate(glm::vec3 translationVector);
        void scale(glm::vec3 scaleVector);
        void rotate(glm::vec3 eulerAngles);
        std::shared_ptr<Utils::Shader> getShader();
        void draw();

    private:

        // Disable Copying and Assignment
        GameObject(GameObject const &) = delete;
        GameObject & operator=(GameObject const &) = delete;

        // Private Member Variables
        glm::mat4 mTransform;
        std::shared_ptr<Utils::Model> mModel;
        std::shared_ptr<Utils::Shader> mShader;
    };
}
