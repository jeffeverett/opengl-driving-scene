#include "Components/physicsbody.hpp"

namespace Components
{
  PhysicsBody::PhysicsBody(Core::GameObject &gameObject) : Component(gameObject)
  {
  }

  PhysicsBody::~PhysicsBody()
  {
  }

  void PhysicsBody::addChildShape(btTransform localTrans, std::shared_ptr<btCollisionShape> childShape)
  {
    mChildShapes.push_back(childShape);
    static_cast<btCompoundShape*>(&(*mShape))->addChildShape(localTrans, &(*childShape));
  }
}