#include "Core/scene.hpp"
#include "Components/script.hpp"

#include <iostream>

namespace Core
{
    void Scene::add(std::shared_ptr<GameObject> gameObject)
    {
        std::cout << "adding gameobject to scene" << std::endl;
        mGameObjects.push_back(gameObject);
        addComponents(gameObject);
    }

    void Scene::addComponents(std::shared_ptr<GameObject> gameObject)
    {
        for (auto it = gameObject->mComponentsHash.begin(); it != gameObject->mComponentsHash.end(); it++) {
            std::cout << "adding component to scene" << std::endl;
            size_t hashCode = it->first;
            if (mComponentsHash.find(hashCode) == mComponentsHash.end()) {
                mComponentsHash[hashCode] = std::vector<std::shared_ptr<Components::Component>>();
            }
            mComponentsHash[hashCode].reserve(mComponentsHash[hashCode].size() + it->second.size());
            mComponentsHash[hashCode].insert(mComponentsHash[hashCode].end(), it->second.begin(), it->second.end());
        }

        for (auto &childGameObject : gameObject->mChildren) {
            addComponents(childGameObject);
        }
    }

    void Scene::update(GLFWwindow *window, float deltaTime)
    {
        for (auto script : getComponents<Components::Script>()) {
            script->onUpdate(window, deltaTime);
        }
    }
}
