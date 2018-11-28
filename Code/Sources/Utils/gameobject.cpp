// Local Headers
#include "Utils/gameobject.hpp"
#include "Utils/scene.hpp"

// System Headers
#include <glm/gtc/matrix_transform.hpp>

// Define Namespace
namespace Utils
{
    glm::vec3 GameObject::getPosition() {
        return mPosition;
    }

    void GameObject::translate(glm::vec3 translationVector) {
        mPosition += translationVector;
    }

    void GameObject::scale(glm::vec3 scaleVector) {
        mScale *= scaleVector;
    }

    void GameObject::rotate(float angle) {
        mTheta += angle;
    }

    std::shared_ptr<Utils::Shader> GameObject::getShader() {
        return mShader;
    }

    void GameObject::draw() {
        // Construct model matrix
        glm::mat4 model(1);
        glm::scale(model, mScale);
        glm::rotate(model, glm::radians(mTheta), glm::vec3(0,1,0));
        glm::translate(model, mPosition);

        mShader->use();
        mShader->setMat4("model", model);
        mDrawable->draw(*mShader);
    }
}
