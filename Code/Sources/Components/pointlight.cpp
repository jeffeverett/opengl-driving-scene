#include "Components/pointlight.hpp"

namespace Components
{
  PointLight::PointLight(Core::GameObject &gameObject) : Light(gameObject)
  {
  }

  PointLight::~PointLight()
  {
  }
}