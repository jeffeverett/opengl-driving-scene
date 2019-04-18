#pragma once

#include "Components/physicsbody.hpp"

namespace Components
{
    class CarPhysicsBody : public PhysicsBody
    {
    public:
        CarPhysicsBody(const Core::GameObject &gameObject);
        virtual ~CarPhysicsBody();

        void applyEngineForce(float force);
        void setBrake(float force);
        void setSteering(float steering);

        std::unique_ptr<btDefaultVehicleRaycaster> mVehicleRaycaster;
        std::unique_ptr<btRaycastVehicle> mVehicle;

        float mSteering;

    private:
        CarPhysicsBody(CarPhysicsBody const &) = delete;
        CarPhysicsBody &operator=(CarPhysicsBody const &) = delete;
    };
}
