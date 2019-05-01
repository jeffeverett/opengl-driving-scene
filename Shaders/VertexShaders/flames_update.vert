#version 400 core

// Inputs
layout (location = 0) in ivec2 aParticleIndices;

// Outputs
flat out ivec2 vParticleIndices;

// Uniforms
uniform int dataTextureWidth;
uniform int dataTextureHeight;

void main()
{
    vParticleIndices = aParticleIndices;
    vec2 xyPos = vec2(float(aParticleIndices.x)/(dataTextureWidth-1), float(aParticleIndices.y)/(dataTextureHeight-1))*2-vec2(1);
    gl_Position = vec4(xyPos, 0, 1);
}