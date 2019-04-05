#include "Components/pointlight.hpp"

namespace Components
{
  PointLight::PointLight(const Core::GameObject &gameObject) : Light(gameObject)
  {
  }

  PointLight::~PointLight()
  {
  }
}