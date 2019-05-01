#version 440 core
layout (location = 0) out vec4 FragColor;

in vec2 vTexCoords;

uniform mat4 view;
uniform sampler2D normalTexture;

void main()
{
    FragColor = vec4(texture(normalTexture, vTexCoords).rgb, 1);
}