#pragma once

#include "Core/transform.hpp"
#include "Components/component.hpp"

#include <vector>
#include <memory>
#include <typeinfo>
#include <unordered_map>

// Forward declaration
namespace Components {
    class Component;
}

namespace Core
{
    class GameObject
    {
    public:
        GameObject(glm::vec3 position = glm::vec3(0));
        ~GameObject();

        void addChild(std::shared_ptr<GameObject> gameObject);
        void addComponent(std::shared_ptr<Components::Component> component);
        
        template <typename T>
        std::vector<std::shared_ptr<T>> getComponents() const
        {
            const std::type_info &ti = typeid(T);
            if (mComponentsHash.find(ti.hash_code()) != mComponentsHash.end()) {
                auto components = mComponentsHash.at(ti.hash_code());
                std::vector<std::shared_ptr<T>> componentsToReturn;
                for (auto component : components) {
                    componentsToReturn.push_back(std::static_pointer_cast<T>(component));
                }
                return componentsToReturn;
            }
            return std::vector<std::shared_ptr<T>>();
        }

        std::shared_ptr<Transform> mTransform;
        std::vector<std::shared_ptr<Components::Component>> mComponents;
        std::vector<std::shared_ptr<GameObject>> mChildren;

    private:
        std::unordered_map<std::size_t, std::vector<std::shared_ptr<Components::Component>>> mComponentsHash; 
    };
}
