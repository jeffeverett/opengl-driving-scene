#include "Utils/openglerrors.hpp"

#include <glad/glad.h>

#include <iostream>

namespace Utils
{
    void OpenGLErrors::checkFramebufferComplete()
    {
      GLenum framebufferStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
      switch (framebufferStatus) {
        case GL_FRAMEBUFFER_UNDEFINED:
          std::cout << "Incomplete framebuffer: framebuffer is undefined. Exiting." << std::endl;
          exit(1);
          break;
        case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
          std::cout << "Incomplete framebuffer: incomplete attachments. Exiting." << std::endl;
          exit(1);
          break;
        case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
          std::cout << "Incomplete framebuffer: missing attachments. Exiting." << std::endl;
          exit(1);
          break;
        case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
          std::cout << "Incomplete framebuffer: incomplete draw buffer. Exiting." << std::endl;
          exit(1);
          break;
        case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
          std::cout << "Incomplete framebuffer: incomplete read buffer. Exiting." << std::endl;
          exit(1);
          break;
        case GL_FRAMEBUFFER_UNSUPPORTED:
          std::cout << "Incomplete framebuffer: invalid internal formats. Exiting." << std::endl;
          exit(1);
          break;
        case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
          std::cout << "Incomplete framebuffer: incomplete multiplesample. Exiting." << std::endl;
          exit(1);
          break;
        case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
          std::cout << "Incomplete framebuffer: incomplete layer targets. Exiting." << std::endl;
          exit(1);
          break;
      }
    }
}