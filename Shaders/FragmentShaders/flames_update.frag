#version 400 core

// Inputs
flat in ivec2 vParticleIndices;

// Outputs
layout (location = 0) out vec4 fPositionTime;

// Uniforms
uniform sampler2D positionTimeMap;

uniform bool isActive;
uniform float deltaTime;
uniform float totalTime;

void main()
{
  fPositionTime = vec4(1,1,1,1);
}
