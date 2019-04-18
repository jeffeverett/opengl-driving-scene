#pragma once

#include "Core/gameobject.hpp"
#include "Rendering/cubemap.hpp"
#include "Rendering/rendersettings.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <vector>
#include <memory>
#include <set>

namespace Core
{
    class Scene
    {
    public:
        Scene() { }
        ~Scene() { }

        void add(std::shared_ptr<Core::GameObject> gameObject);
        void update(GLFWwindow *window, float deltaTime);

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

        std::vector<std::shared_ptr<Core::GameObject>> mGameObjects;

        Rendering::CubeMap mCubeMap;
        Rendering::RenderSettings mRenderSettings;

    private:
        Scene(Scene const &) = delete;
        Scene & operator=(Scene const &) = delete;

        void addComponents(std::shared_ptr<GameObject> gameObject);

        std::unordered_map<size_t, std::vector<std::shared_ptr<Components::Component>>> mComponentsHash;
    };
}
