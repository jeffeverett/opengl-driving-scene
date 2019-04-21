#pragma once

#include <glm/glm.hpp>
#include <bullet/btBulletDynamicsCommon.h>

#include <string>
#include <iostream>

namespace Utils
{
  class Logger
  {
    public:
      template <typename T>
      static void log(std::string message, T obj)
      {
        std::cout << message << ": " << toString(obj) << std::endl;
      }

    private:
      static std::string toString(glm::vec3 obj);
      static std::string toString(btVector3 obj);

      static std::string toString(glm::mat4);
  };
}