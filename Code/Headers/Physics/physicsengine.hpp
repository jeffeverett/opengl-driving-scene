#pragma once

#include "Core/scene.hpp"

#include <bullet/btBulletDynamicsCommon.h>

namespace Physics
{
    class PhysicsEngine
    {
    public:
        PhysicsEngine();
        ~PhysicsEngine();

        void updateScene(Core::Scene const &scene, double deltaTime);
        void updateDirtyTransforms(std::shared_ptr<Core::GameObject> gameObject, glm::mat4 matrix, bool parentDirty);
        void connectDebugRenderer(btIDebugDraw *debugRenderer);

        std::unique_ptr<btDiscreteDynamicsWorld> mDynamicsWorld;

    private:
        PhysicsEngine(PhysicsEngine const &) = delete;
        PhysicsEngine & operator=(PhysicsEngine const &) = delete;
    };
}
