#version 440 core
layout (location = 0) out vec4 FragColor;

in vec2 vTexCoords;

uniform sampler2D albedoSpecTexture;

void main()
{
    float spec = texture(albedoSpecTexture, vTexCoords).a;
    FragColor = vec4(spec, spec, spec, 1);
}