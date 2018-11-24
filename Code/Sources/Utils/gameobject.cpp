// Local Headers
#include "Utils/gameobject.hpp"
#include "Utils/scene.hpp"

// Define Namespace
namespace Utils
{
    void GameObject::translate(glm::vec3 translationVector) {
        mTransform = glm::translate(mTransform, translationVector);
    }

    void GameObject::scale(glm::vec3 scaleVector) {
        mTransform = glm::translate(mTransform, scaleVector);
    }

    void GameObject::rotate(glm::vec3 eulerAngles) {
        mTransform = glm::translate(mTransform, eulerAngles);
    }

    std::shared_ptr<Utils::Shader> GameObject::getShader() {
        return mShader;
    }

    void GameObject::draw() {
        //fprintf(stdout, "Drawing object.\n");
        mShader->use();
        mShader->setMat4("model", mTransform);
        mModel->Draw(*mShader);
    }
}
