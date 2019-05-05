#version 440 core

// Inputs
in vec3 gPosition;
in vec3 gColor;
in vec2 gTexCoords;

// Outputs
out vec4 fFragColor;

// Uniforms
uniform sampler2D albedoMap0; // Flames

void main()
{
  vec4 sampledMap = texture(albedoMap0, gTexCoords);
  float alpha = (sampledMap.r + sampledMap.g + sampledMap.b) / 3;
  fFragColor = vec4(sampledMap.rgb*gColor, alpha*0.1);
}
