#version 330 core
layout (location = 0) in vec2 aPos;

out vec2 TexCoords;

uniform vec2 scale;
uniform vec2 offset;

void main()
{
    TexCoords = (aPos + 1.0) / 2.0;
    gl_Position = vec4(scale * aPos + offset, 0.0, 1.0);
}