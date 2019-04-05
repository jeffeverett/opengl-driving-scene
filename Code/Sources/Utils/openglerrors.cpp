#include "Utils/openglerrors.hpp"

#include <glad/glad.h>

#include <iostream>

namespace Utils
{
    void OpenGLErrors::checkFramebufferComplete()
    {
      GLenum framebufferStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
      switch (framebufferStatus) {
        case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
          std::cout << "Incomplete framebuffer: incomplete attachments. Exiting." << std::endl;
          exit(1);
          break;
        case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS:
          std::cout << "Incomplete framebuffer: non-matching dimensions. Exiting." << std::endl;
          exit(1);
          break;
        case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
          std::cout << "Incomplete framebuffer: missing attachments. Exiting." << std::endl;
          exit(1);
          break;
        case GL_FRAMEBUFFER_UNSUPPORTED:
          std::cout << "Incomplete framebuffer: invalid internal formats. Exiting." << std::endl;
          exit(1);
          break;
      }
    }
}