#include "Core/transform.hpp"

#include <glm/gtc/matrix_transform.hpp>

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
  
  void Transform::scale(const glm::vec3 &scaleVector)
  {
    mScale *= scaleVector;
    mIsDirty = true;
  }
  
  void Transform::translate(const glm::vec3 &translationVector)
  {
    mTranslation += translationVector;
    mIsDirty = true;
  }

  void Transform::rotate(const glm::quat &quaternion)
  {
    mRotation = quaternion;
    mIsDirty = true;
  }

  void Transform::updateModelMatrix(glm::mat4 startingMatrix)
  {
    // Update model matrix
    glm::mat4 matrix = glm::scale(startingMatrix, mScale);
    matrix = glm::translate(matrix, mTranslation);
    glm::mat4 rotationMatrix = glm::toMat4(mRotation);
    mModelMatrix = rotationMatrix * matrix;

    std::cout << "Element 0 is " << matrix[0][0] << std::endl;

    // No longer dirty
    mIsDirty = false;
  }
}