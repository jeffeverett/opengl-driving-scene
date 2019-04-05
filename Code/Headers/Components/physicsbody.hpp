#pragma once

#include "Components/component.hpp"

#include <bullet/btBulletDynamicsCommon.h>

namespace Components
{
    class PhysicsBody : public Component
    {
    public:
        PhysicsBody(const Core::GameObject &gameObject);
        virtual ~PhysicsBody();

        void addChildShape(btTransform localTrans, std::shared_ptr<btCollisionShape> childShape);

        std::unique_ptr<btRigidBody> mRigidBody;
        std::unique_ptr<btCollisionShape> mShape;
        std::vector<std::shared_ptr<btCollisionShape>> mChildShapes;
        std::unique_ptr<btDefaultMotionState> mMotionState;
    private:
        PhysicsBody(PhysicsBody const &) = delete;
        PhysicsBody &operator=(PhysicsBody const &) = delete;
    };
}
