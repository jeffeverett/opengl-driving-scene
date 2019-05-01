#version 440 core

layout(local_size_x=1, local_size_y=1, local_size_z=1) in;

// Buffers
layout(binding=6) buffer positionsBuffer {
  vec4 positions[];
};
layout(binding=7) buffer velocitiesBuffer {
  vec4 velocities[];
};
layout(binding=8) buffer colorsBuffer {
  vec4 colors[];
};

// Uniforms
uniform mat4 model;

uniform int numParticles;

uniform float particleLifetime;
uniform float totalTime;
uniform float deltaTime;

uniform vec3 color0; // Initial color
uniform vec3 color1; // Final color

void main()
{
  uint gid = gl_GlobalInvocationID.x;
  
  positions[gid].w -= deltaTime;

  // w of "positions" encodes the amount of time remaining in particle lifetime
  if (positions[gid].w <= 0) {
    float timeIntoCycle = mod(totalTime, particleLifetime);
    float currPartCyclePlacement = float(gid) / numParticles * particleLifetime;
    if (totalTime > currPartCyclePlacement && totalTime <= particleLifetime) {
      positions[gid].xyz = vec3(model * vec4(positions[gid].xyz,1));
      positions[gid].w = particleLifetime;

      velocities[gid].xyz = vec3(0, 1, 0);
      velocities[gid].w = 0;

      colors[gid].rgb = color0;
      colors[gid].a = 1;
    }
  }
  else {
    positions[gid].xyz += velocities[gid].xyz * deltaTime;

    velocities[gid].xyz *= 0.99;

    float timeIntoLifetime = (particleLifetime-positions[gid].w)/particleLifetime;
    colors[gid].xyz = mix(color0, color1, timeIntoLifetime);
  }
}
