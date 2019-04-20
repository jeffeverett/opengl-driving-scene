#include "Components/light.hpp"

namespace Components
{
  Light::Light(const Core::GameObject &gameObject) : Component(gameObject)
  {
  }

  Light::~Light()
  {
  }

  glm::vec3 Light::getDirection()
  {
    auto modelMatrix = mGameObject.mTransform->mModelMatrix;
    glm::vec3 lightDirection(modelMatrix[0][2], modelMatrix[1][2], modelMatrix[2][2]);
    return glm::normalize(lightDirection);
  }
}