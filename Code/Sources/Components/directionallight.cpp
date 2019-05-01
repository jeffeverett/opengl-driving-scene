#include "Components/directionallight.hpp"

namespace Components
{
  DirectionalLight::DirectionalLight(Core::GameObject &gameObject) : Light(gameObject)
  {
  }

  DirectionalLight::~DirectionalLight()
  {
  }
}