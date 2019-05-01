#version 440 core

// Inputs
layout (location = 0) in vec2 aParticleIndices;

// Outputs
out vec3 vPosition;
out vec2 vParticleIndices;

// Uniforms
uniform sampler2D positionTimeMap;

uniform mat4 view;
uniform mat4 projection;

void main()
{
    vPosition = textureOffset(positionTimeMap, vec2(0), ivec2(aParticleIndices)).rgb;
    vParticleIndices = aParticleIndices;
    gl_Position = projection * view * vec4(vPosition, 1);
}