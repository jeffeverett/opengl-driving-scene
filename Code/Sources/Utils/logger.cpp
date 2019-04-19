#include "Utils/logger.hpp"

#include <sstream>

namespace Utils
{
  std::string Logger::toString(glm::vec3 obj)
  {
    std::stringstream ss;
    ss << "(" << obj.x << ", " << obj.y << ", " << obj.z << ")";
    return ss.str();
  }

  std::string Logger::toString(btVector3 obj)
  {
    std::stringstream ss;
    ss << "(" << obj[0] << ", " << obj[1] << ", " << obj[2] << ")";
    return ss.str();
  }
}