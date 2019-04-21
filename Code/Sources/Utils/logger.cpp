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

  std::string Logger::toString(glm::mat4 obj)
  {
    std::stringstream ss;
    ss << "\n  [" << obj[0][0] << ", " << obj[0][1] << ", " << obj[0][2] << ", " << obj[0][3] << ",\n"
       << "   "  << obj[1][0] << ", " << obj[1][1] << ", " << obj[1][2] << ", " << obj[2][3] << ",\n"
       << "   "  << obj[2][0] << ", " << obj[2][1] << ", " << obj[2][2] << ", " << obj[2][3] << ",\n"
       << "   "  << obj[3][0] << ", " << obj[3][1] << ", " << obj[3][2] << ", " << obj[3][3] << "]";
    return ss.str();
  }
}