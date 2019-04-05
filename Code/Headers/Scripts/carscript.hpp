#pragma once

#include "Components/script.hpp"

namespace Scripts
{
    class CarScript : public Components::Script
    {
    public:
        CarScript(Core::GameObject const &);
        virtual ~CarScript();

        virtual void onStart() override;
        virtual void onUpdate() override;
    };
}
