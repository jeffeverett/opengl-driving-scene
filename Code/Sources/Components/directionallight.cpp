#include "Components/directionallight.hpp"

namespace Components
{
  DirectionalLight::DirectionalLight(const Core::GameObject &gameObject) : Light(gameObject)
  {
  }

  DirectionalLight::~DirectionalLight()
  {
  }
}