#pragma once

#include <vector>
#include <memory>
#include <set>

namespace Rendering
{
  enum RenderMode {
    DEFERRED_SHADING,
    DEBUG 
  };

  struct RenderSettings {
    RenderMode mRenderMode;
    bool mPhysicsDebug;
    
    float mScreenWidth;
    float mScreenHeight;
  };
}
