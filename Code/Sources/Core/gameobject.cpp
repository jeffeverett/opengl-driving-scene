#include "Core/gameobject.hpp"

#include <iostream>

namespace Core
{
    GameObject::GameObject(glm::vec3 position) : mTransform(std::make_shared<Transform>())
    {
        mTransform->translate(position);
    }

    GameObject::~GameObject()
    {
    }

    void GameObject::addChild(std::shared_ptr<GameObject> gameObject)
    {
        mChildren.push_back(gameObject);
    }
    
    void GameObject::addComponent(std::shared_ptr<Components::Component> component)
    {
        mComponents.push_back(component);

        const std::type_info &ti = typeid(*component);
        std::cout << ti.name() << std::endl;
        if (mComponentsHash.find(ti.hash_code()) == mComponentsHash.end()) {
            mComponentsHash[ti.hash_code()] = std::vector<std::shared_ptr<Components::Component>>();
        }
        mComponentsHash[ti.hash_code()].push_back(component);
    }
}
