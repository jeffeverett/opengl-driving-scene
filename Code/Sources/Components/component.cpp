#include "Components/component.hpp"


namespace Components
{
  Component::Component(Core::GameObject &gameObject) : mGameObject(gameObject)
  {
  }

  Component::~Component()
  {
  }
}