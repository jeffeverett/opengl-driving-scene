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
    Component(Core::GameObject &gameObject);
    virtual ~Component();

    Core::GameObject &mGameObject;
  };
}