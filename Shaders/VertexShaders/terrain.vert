#version 400

// Inputs
layout (location = 0) in vec2 aPos;

// Outputs
out vec3 vPosition;
out vec2 vTexCoords;

// Uniforms
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform float heightScale;

uniform int gridSizeX;
uniform int gridSizeZ;

uniform int startX;
uniform int startZ;

uniform sampler2D heightMap;

void main()
{
   // Determine UV
   vTexCoords = vec2((startX+gridSizeX/2+aPos.x)/gridSizeX, (startZ+gridSizeZ/2+aPos.y)/gridSizeZ);
   
   // Determine position
   float height = texture(heightMap, vTexCoords).r * heightScale;
   vPosition = vec3(startX+aPos.x, height, startZ+aPos.y);
}
