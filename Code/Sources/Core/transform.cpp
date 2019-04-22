#include "Core/transform.hpp"
#include "Utils/logger.hpp"

#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>

#include <iostream>

namespace Core
{
  Transform::Transform() : mScale(glm::vec3(1)), mTranslation(glm::vec3(0)), mRotation(glm::vec3(0)),
    mModelMatrix(glm::mat4(1)), mIsDirty(false)
  {
  }

  Transform::~Transform()
  {
  }
  
  void Transform::setScale(const glm::vec3 &scaleVector)
  {
    mScale = scaleVector;
    mIsDirty = true;
  }
  
  void Transform::setTranslation(const glm::vec3 &translationVector)
  {
    mTranslation = translationVector;
    mIsDirty = true;
  }

  void Transform::setRotation(const glm::quat &quaternion)
  {
    mRotation = quaternion;
    mIsDirty = true;
  }

  void Transform::setRotation(const glm::vec3 &eulerAngles)
  {
    mRotation = glm::toQuat(glm::orientate3(eulerAngles));
    mIsDirty = true;
  }

  glm::vec3 Transform::getWorldTranslation()
  {
    return glm::vec3(mModelMatrix[3][0], mModelMatrix[3][1], mModelMatrix[3][2]);
  }

  void Transform::updateModelMatrix(glm::mat4 startingMatrix)
  {
    // Update model matrix
    glm::mat4 mtx(1);
    mtx = glm::scale(mtx, mScale);
    glm::mat4 rotationMatrix = glm::toMat4(mRotation);
    mtx = rotationMatrix * mtx;
    mtx[3][0] += mTranslation.x;
    mtx[3][1] += mTranslation.y;
    mtx[3][2] += mTranslation.z;
    mModelMatrix = mtx * startingMatrix;

    //Utils::Logger::log("Starting matrix", startingMatrix);
    //Utils::Logger::log("Model matrix", mModelMatrix);

    // No longer dirty
    mIsDirty = false;
  }
}