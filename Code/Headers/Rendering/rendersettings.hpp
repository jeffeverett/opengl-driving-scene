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

  enum TerrainRenderMode {
    ALBEDO,
    ALBEDO_AND_WIREFRAME,
    WIREFRAME
  };

  struct RenderSettings {
    RenderMode mRenderMode;
    TerrainRenderMode mTerrainRenderMode;
    bool mDrawDebugLines;
    bool mUseFXAA;
    
    float mFramebufferWidth;
    float mFramebufferHeight;
  };
}
