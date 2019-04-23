#version 400 core

// Inputs
layout (location = 0) in vec2 aPos;

// Outputs
out vec3 vPosition;
out vec2 vTexCoords;

// Uniforms
uniform float heightScale;

uniform int gridSizeX;
uniform int gridSizeZ;

uniform int startX;
uniform int startZ;

uniform float scaleX;
uniform float scaleZ;

uniform sampler2D heightMap;

void main()
{
   // Determine UV
   vTexCoords = vec2((startX+gridSizeX/2+aPos.x*scaleX)/gridSizeX, (startZ+gridSizeZ/2+aPos.y*scaleZ)/gridSizeZ);
   
   // Determine position
   float height = texture(heightMap, vTexCoords).r * heightScale - heightScale/2;
   vPosition = vec3(startX+aPos.x*scaleX, height, startZ+aPos.y*scaleZ);
}
