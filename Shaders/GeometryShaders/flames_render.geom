#version 400 core

// Triangles in and out
layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

// Input
in vec3 vPosition[1];
in vec2 vParticleIndices[1];

// Output
out vec3 gPosition;
out vec2 gTexCoords;

// Uniforms
uniform sampler2D positionTimeMap;

uniform mat4 view;
uniform mat4 projection;

uniform vec2 particleSize;

void main()
{
  // Do not spawn any vertices if particle is not alive
  if (texture(positionTimeMap, vParticleIndices[0]).a <= 0) {
    return;
  }

  // Determine up and right vectors of camera for billboarding
  vec3 up = vec3(view[0][1], view[1][1], view[2][1]);
  vec3 right = vec3(view[0][0], view[1][0], view[2][0]);

  // Bottom-left vertex
  gPosition = vPosition[0] - particleSize[0]/2*right - particleSize[1]/2*up;
  gTexCoords = vec2(0, 0);
  gl_Position = projection * view * vec4(gPosition, 1);
  EmitVertex();

  // Bottom-right vertex
  gPosition = vPosition[0] + particleSize[0]/2*right - particleSize[1]/2*up;
  gTexCoords = vec2(1, 0);
  gl_Position = projection * view * vec4(gPosition, 1);
  EmitVertex();

  // Top-left vertex
  gPosition = vPosition[0] - particleSize[0]/2*right + particleSize[1]/2*up;
  gTexCoords = vec2(0, 1);
  gl_Position = projection * view * vec4(gPosition, 1);
  EmitVertex();

  // Top-right vertex
  gPosition = vPosition[0] + particleSize[0]/2*right + particleSize[1]/2*up;
  gTexCoords = vec2(1, 1);
  gl_Position = projection * view * vec4(gPosition, 1);
  EmitVertex();

  // Done with quad
  EndPrimitive();
}
