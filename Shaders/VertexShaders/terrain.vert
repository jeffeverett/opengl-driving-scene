#version 400 core

// Inputs
layout (location = 0) in vec2 aPos;
layout (location = 1) in float aStartX;
layout (location = 2) in float aStartZ;

// Outputs
out vec3 vPosition;
out vec2 vTexCoords;

// Uniforms
uniform float heightScale;

uniform int gridSizeX;
uniform int gridSizeZ;

uniform float scaleX;
uniform float scaleZ;

uniform sampler2D heightMap;

void main()
{
   // Determine UV
   vTexCoords = vec2((aStartX+gridSizeX/2+aPos.x*scaleX)/gridSizeX, (aStartZ+gridSizeZ/2+aPos.y*scaleZ)/gridSizeZ);
   
   // Determine position
   float height = texture(heightMap, vTexCoords).r * heightScale - heightScale/2;
   vPosition = vec3(aStartX+aPos.x*scaleX, height, aStartZ+aPos.y*scaleZ);
}
