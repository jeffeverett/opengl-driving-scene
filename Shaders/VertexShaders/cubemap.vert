#version 440 core

// Inputs
layout (location = 0) in vec3 aPos;

// Outputs
out vec3 vTexCoords;

// Uniforms
uniform mat4 projection;
uniform mat4 view;

void main()
{
    vTexCoords = aPos;

    // Strip out translation from view matrix
    mat4 viewMtx = mat4(mat3(view));
    gl_Position = projection * viewMtx * vec4(aPos, 1.0);
}