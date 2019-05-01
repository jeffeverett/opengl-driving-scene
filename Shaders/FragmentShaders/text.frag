#version 440 core

// Inputs
in vec2 vTexCoords;

// Outputs
out vec4 fFragColor;

// Uniforms
uniform sampler2D text;
uniform vec3 textColor;

void main()
{
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, vTexCoords).r);
    fFragColor = vec4(textColor,1.0)*sampled;
}