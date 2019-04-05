#include "Core/scene.hpp"
#include "Components/script.hpp"

#include <iostream>

namespace Core
{
    void Scene::add(std::shared_ptr<GameObject> gameObject)
    {
        mGameObjects.push_back(gameObject);
        addComponents(gameObject);
    }

    void Scene::addComponents(std::shared_ptr<GameObject> gameObject)
    {
        for (auto &component : gameObject->mComponents) {
            std::cout << "adding component" << std::endl;
            const std::type_info &ti = typeid(*component);
            if (mComponentsHash.find(ti.hash_code()) == mComponentsHash.end()) {
                mComponentsHash[ti.hash_code()] = std::vector<std::shared_ptr<Components::Component>>();
            }
            mComponentsHash[ti.hash_code()].push_back(component);
        }

        for (auto &childGameObject : gameObject->mChildren) {
            addComponents(childGameObject);
        }
    }

    void Scene::update(float deltaTime)
    {
        for (auto gameObject : mGameObjects) {
            auto scripts = gameObject->getComponents<Components::Script>();
            for (auto script : scripts) {
                script->onUpdate();
            }
        }
    }
}
