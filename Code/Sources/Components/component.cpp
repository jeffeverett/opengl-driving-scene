#include "Components/component.hpp"


namespace Components
{
  Component::Component(const Core::GameObject &gameObject) : mGameObject(gameObject)
  {
  }

  Component::~Component()
  {
  }
}