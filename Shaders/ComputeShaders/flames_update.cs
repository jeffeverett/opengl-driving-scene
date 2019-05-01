#version 440 core

layout(local_size_x=1, local_size_y=1, local_size_z=1) in;

// Uniforms
layout(rgba32f) uniform image2D positionTimeMap;

void main()
{
  ivec2 idx = ivec2(gl_WorkGroupID.x, gl_WorkGroupID.y);
}
