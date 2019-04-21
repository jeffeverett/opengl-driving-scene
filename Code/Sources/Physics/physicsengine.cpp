#include "Physics/physicsengine.hpp"
#include "Components/physicsbody.hpp"
#include "Components/carphysicsbody.hpp"
#include "Utils/transformconversions.hpp"


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
      mDynamicsWorld->stepSimulation((float) deltaTime, 5);

      // ***** UPDATE TRANSFORMS ****
      int i = 0;
      for (auto &physicsBody : scene.getComponents<Components::PhysicsBody>()) {
        auto physicsBodyTransform = physicsBody->mRigidBody->getWorldTransform();
        auto gameObjectTransform = physicsBody->mGameObject.mTransform;
        
        btScalar transform[16];
        physicsBodyTransform.getOpenGLMatrix(transform);
        transform[0] *= gameObjectTransform->mScale.x;
        transform[5] *= gameObjectTransform->mScale.y;
        transform[10] *= gameObjectTransform->mScale.z;

        gameObjectTransform->mTranslation = Utils::TransformConversions::btVector32glmVec3(physicsBodyTransform.getOrigin());

        std::cout << scene.getComponents<Components::PhysicsBody>().size() << std::endl;
        std::cout << i++ << " " << gameObjectTransform->mTranslation.x << " " << gameObjectTransform->mTranslation.y << " " << gameObjectTransform->mTranslation.z << std::endl;

        gameObjectTransform->mRotation = Utils::TransformConversions::btQuaternion2glmQuat(physicsBodyTransform.getRotation());
        gameObjectTransform->mModelMatrix = Utils::TransformConversions::btScalar2glmMat4(transform);
        for (auto &gameObject : physicsBody->mGameObject.mChildren) {
          updateDirtyTransforms(gameObject, gameObjectTransform->mModelMatrix, false);
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
