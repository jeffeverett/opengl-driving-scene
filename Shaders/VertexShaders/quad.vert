#version 400 core
layout (location = 0) in vec2 aPos;

out vec2 vTexCoords;

uniform vec2 scale;
uniform vec2 offset;

void main()
{
    vTexCoords = (aPos + 1.0) / 2.0;
    gl_Position = vec4(scale * aPos + offset, 0.0, 1.0);
}