#pragma once

#include "Components/component.hpp"

namespace Components
{
    class Text : public Component
    {
    public:
        Text(const Core::GameObject &gameObject);
        virtual ~Text();

        std::string mText;
        float mLocation[2];

    private:
        Text(Text const &) = delete;
        Text &operator=(Text const &) = delete;
    };
}
