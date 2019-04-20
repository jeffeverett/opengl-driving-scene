#include "Core/gameobject.hpp"

namespace Core
{
    GameObject::GameObject(glm::vec3 position) : mTransform(std::make_shared<Transform>())
    {
        mTransform->setTranslation(position);
    }

    GameObject::~GameObject()
    {
    }

    void GameObject::addChild(std::shared_ptr<GameObject> gameObject)
    {
        gameObject->mTransform->updateModelMatrix(mTransform->mModelMatrix);
        mChildren.push_back(gameObject);
    }
}
