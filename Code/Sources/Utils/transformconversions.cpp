#include "Utils/transformconversions.hpp"

namespace Utils
{
    glm::mat4 TransformConversions::btScalar2glmMat4(btScalar* matrix)
    {
        return glm::mat4(
                matrix[0], matrix[1], matrix[2], matrix[3],
                matrix[4], matrix[5], matrix[6], matrix[7],
                matrix[8], matrix[9], matrix[10], matrix[11],
                matrix[12], matrix[13], matrix[14], matrix[15]);
    }

    glm::vec3 TransformConversions::btVector32glmVec3(btVector3 vec)
    {
        return glm::vec3(vec[0], vec[1], vec[2]);
    }

    btVector3 TransformConversions::glmVec32btVector3(glm::vec3 vec)
    {
        return btVector3(vec[0], vec[1], vec[2]);
    }
}