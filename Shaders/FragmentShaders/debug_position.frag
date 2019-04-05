#version 330 core
layout (location = 0) out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D positionTexture;

void main()
{
    FragColor = vec4(texture(positionTexture, TexCoords).rgb, 1);
}