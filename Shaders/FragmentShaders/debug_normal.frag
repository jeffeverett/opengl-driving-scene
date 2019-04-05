#version 400 core
layout (location = 0) out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D normalTexture;

void main()
{
    FragColor = vec4(texture(normalTexture, TexCoords).rgb, 1);
}