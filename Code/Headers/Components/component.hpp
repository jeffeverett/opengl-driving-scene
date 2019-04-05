#pragma once

#include "Core/gameobject.hpp"

// Forward declaration
namespace Core {
    class GameObject;
}

namespace Components
{
  class Component
  {
  public:
    Component(const Core::GameObject &gameObject);
    virtual ~Component();

    const Core::GameObject &mGameObject;
  };
}