#version 400 core

// Inputs
in vec3 vTexCoords;

// Outputs
out vec4 fFragColor;

// Uniforms
uniform samplerCube skybox;

void main()
{
    fFragColor = texture(skybox, vTexCoords);
}