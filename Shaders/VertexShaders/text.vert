#version 440 core

// Inputs
layout (location = 0) in vec4 aVertex; // <vec2 pos, vec2 tex>

// Outputs
out vec2 vTexCoords;

// Uniforms
uniform mat4 projection;

void main()
{
    gl_Position = projection * vec4(aVertex.xy, 0.0, 1.0);
    vTexCoords = aVertex.zw;
}