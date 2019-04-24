#version 400 core
layout (location = 0) out vec4 FragColor;

in vec2 vTexCoords;

uniform sampler2D albedoSpecTexture;

void main()
{
    FragColor = vec4(texture(albedoSpecTexture, vTexCoords).rgb, 1);
}