#include "Components/carphysicsbody.hpp"

const float STEERING_CLAMP = 0.3f;


namespace Components
{
  CarPhysicsBody::CarPhysicsBody(Core::GameObject &gameObject) : PhysicsBody(gameObject)
  {
  }

  CarPhysicsBody::~CarPhysicsBody()
  {
  }

  void CarPhysicsBody::applyEngineForce(float force)
  {
    for (int i=0; i < mVehicle->getNumWheels(); i++) {
      btWheelInfo &wheel = mVehicle->getWheelInfo(i);
      // 4WD
      mVehicle->applyEngineForce(force, i);
    }
  }

  void CarPhysicsBody::setBrake(float force)
  {
    for (int i=0; i < mVehicle->getNumWheels(); i++) {
      btWheelInfo &wheel = mVehicle->getWheelInfo(i);
      // 4WD
      mVehicle->setBrake(force, i);
    }
  }

  void CarPhysicsBody::setSteering(float steering)
  {
    mSteering = steering;
    if (mSteering > STEERING_CLAMP) {
      mSteering = STEERING_CLAMP;
    }
    else if (mSteering < -STEERING_CLAMP) {
      mSteering = -STEERING_CLAMP;
    }
    for (int i=0; i < mVehicle->getNumWheels(); i++) {
      btWheelInfo &wheel = mVehicle->getWheelInfo(i);
      // Steer with front wheels
      if (wheel.m_bIsFrontWheel) {
        mVehicle->setSteeringValue(mSteering, i);
      }
    }
  }
}