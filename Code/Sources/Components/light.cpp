#include "Components/light.hpp"

namespace Components
{
  Light::Light(Core::GameObject &gameObject) : Component(gameObject)
  {
  }

  Light::~Light()
  {
  }

  glm::vec3 Light::getDirection()
  {
    auto modelMatrix = mGameObject.mTransform->mModelMatrix;
    glm::vec3 lightDirection(modelMatrix[2][0], modelMatrix[2][1], modelMatrix[2][2]);
    return glm::normalize(lightDirection);
  }
}