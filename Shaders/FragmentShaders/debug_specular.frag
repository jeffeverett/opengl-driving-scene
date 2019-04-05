#version 400 core
layout (location = 0) out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D albedoSpecTexture;

void main()
{
    float spec = texture(albedoSpecTexture, TexCoords).a;
    FragColor = vec4(spec, spec, spec, 1);
}