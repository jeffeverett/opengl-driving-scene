#include "Physics/physicsengine.hpp"
#include "Components/physicsbody.hpp"
#include "Components/carphysicsbody.hpp"
#include "Utils/transformconversions.hpp"
#include "Utils/logger.hpp"

#include <glm/gtc/matrix_transform.hpp>

#include <iostream>

namespace Physics
{
    PhysicsEngine::PhysicsEngine()
    {
      mCollisionConfiguration = std::make_unique<btDefaultCollisionConfiguration>();
      mDispatcher = std::make_unique<btCollisionDispatcher>(&(*mCollisionConfiguration));
      mOverlappingPairCache = std::make_unique<btDbvtBroadphase>();
      mSolver = std::make_unique<btSequentialImpulseConstraintSolver>();
      mDynamicsWorld = std::make_unique<btDiscreteDynamicsWorld>(&(*mDispatcher), &(*mOverlappingPairCache), &(*mSolver), &(*mCollisionConfiguration));
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

      // ***** STEP SCENE *****
      //mDynamicsWorld->stepSimulation((float) deltaTime, 5);

      // ***** UPDATE TRANSFORMS ****
      int i = 0;
      for (auto &physicsBody : scene.getComponents<Components::PhysicsBody>()) {
        auto physicsBodyTransform = physicsBody->mRigidBody->getWorldTransform();
        auto gameObjectTransform = physicsBody->mGameObject.mTransform;

        gameObjectTransform->mTranslation = Utils::TransformConversions::btVector32glmVec3(physicsBodyTransform.getOrigin());
        gameObjectTransform->mRotation = Utils::TransformConversions::btQuaternion2glmQuat(physicsBodyTransform.getRotation());

        btScalar transform[16];
        physicsBodyTransform.getOpenGLMatrix(transform);
        glm::mat4 translateRotateMtx = Utils::TransformConversions::btScalar2glmMat4(transform);
        glm::mat4 scaleMtx = glm::scale(glm::mat4(1), gameObjectTransform->mScale);
        gameObjectTransform->mModelMatrix = translateRotateMtx * scaleMtx;

        for (auto &gameObject : physicsBody->mGameObject.mChildren) {
          updateDirtyTransforms(gameObject, gameObjectTransform->mModelMatrix, true);
        }
      }
    }

    void PhysicsEngine::updateDirtyTransforms(std::shared_ptr<Core::GameObject> gameObject, glm::mat4 matrix, bool parentDirty)
    {
      // Update model matrix and physics bodies
      bool isDirty = parentDirty || gameObject->mTransform->mIsDirty;
      if (isDirty) {
        gameObject->mTransform->updateModelMatrix(matrix);

        auto physicsBodies = gameObject->getComponents<Components::PhysicsBody>();
        for (auto physicsBody : physicsBodies) {
          physicsBody->mRigidBody->getWorldTransform().setRotation(
            Utils::TransformConversions::glmQuat2btQuaternion(gameObject->mTransform->mRotation));
          physicsBody->mRigidBody->getWorldTransform().setOrigin(
            Utils::TransformConversions::glmVec32btVector3(gameObject->mTransform->getWorldTranslation()));
        }
      }

      // Update children
      for (auto &childGameObject : gameObject->mChildren) {
        updateDirtyTransforms(childGameObject, gameObject->mTransform->mModelMatrix, isDirty);
      }
    }

    void PhysicsEngine::connectDebugRenderer(btIDebugDraw *debugRenderer)
    {
      mDynamicsWorld->setDebugDrawer(debugRenderer);
    }
}
