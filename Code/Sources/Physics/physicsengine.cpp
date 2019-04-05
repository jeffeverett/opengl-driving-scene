#include "Physics/physicsengine.hpp"

#include <iostream>

namespace Physics
{
    PhysicsEngine::PhysicsEngine()
    {
      btDefaultCollisionConfiguration collisionConfiguration;
      btCollisionDispatcher dispatcher(&collisionConfiguration);
      btDbvtBroadphase overlappingPairCache;
      btSequentialImpulseConstraintSolver solver;
      mDynamicsWorld = std::make_unique<btDiscreteDynamicsWorld>(&dispatcher, &overlappingPairCache, &solver, &collisionConfiguration);
    }

    PhysicsEngine::~PhysicsEngine()
    {
    }

    void PhysicsEngine::updateScene(Core::Scene const &scene, double deltaTime)
    {
      // ***** UPDATE DIRTY TRANSFORMS *****
      for (auto &gameObject : scene.mGameObjects) {
        updateDirtyTransforms(gameObject, glm::mat4(1), false);
      }
      //mDynamicsWorld->stepSimulation((float) deltaTime, 5);
    }

    void PhysicsEngine::updateDirtyTransforms(std::shared_ptr<Core::GameObject> gameObject, glm::mat4 matrix, bool parentDirty)
    {
      bool isDirty = parentDirty || gameObject->mTransform->mIsDirty;
      if (isDirty) {
        gameObject->mTransform->updateModelMatrix(matrix);
      }

      for (auto &childGameObject : gameObject->mChildren) {
        updateDirtyTransforms(childGameObject, gameObject->mTransform->mModelMatrix, isDirty);
      }
    }

    void PhysicsEngine::connectDebugRenderer(btIDebugDraw *debugRenderer)
    {
      mDynamicsWorld->setDebugDrawer(debugRenderer);
    }
}
