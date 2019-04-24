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

  enum FXAARenderMode {
    FXAA,
    FXAA_AND_DEBUG,
    NONE
  };

  struct RenderSettings {
    RenderMode mRenderMode;
    TerrainRenderMode mTerrainRenderMode;
    FXAARenderMode mFXAARenderMode;
    bool mDrawDebugLines;
    
    float mFramebufferWidth;
    float mFramebufferHeight;
  };
}
