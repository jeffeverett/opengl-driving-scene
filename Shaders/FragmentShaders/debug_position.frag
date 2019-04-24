#version 400 core
layout (location = 0) out vec4 FragColor;

in vec2 vTexCoords;

uniform sampler2D positionTexture;

void main()
{
    FragColor = vec4(texture(positionTexture, vTexCoords).rgb, 1);
}