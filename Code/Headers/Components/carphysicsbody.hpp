#pragma once

#include "Components/physicsbody.hpp"

namespace Components
{
    class CarPhysicsBody : public PhysicsBody
    {
    public:
        CarPhysicsBody(const Core::GameObject &gameObject);
        virtual ~CarPhysicsBody();

        std::unique_ptr<btDefaultVehicleRaycaster> mVehicleRaycaster;
        std::unique_ptr<btRaycastVehicle> mVehicle;

    private:
        CarPhysicsBody(CarPhysicsBody const &) = delete;
        CarPhysicsBody &operator=(CarPhysicsBody const &) = delete;
    };
}
