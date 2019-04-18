#version 400 core
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;

in vec3 vFragPos;
in vec2 vTexCoords;
flat in mat3 vTBN;

uniform sampler2D albedoMap;
uniform sampler2D normalMap;
uniform sampler2D specularMap;


void main()
{    
    // Store the fragment position vector
    gPosition = vTBN[2];
    // Store the per-fragment normals
    gNormal = vTBN*texture(normalMap, vTexCoords).rgb;
    // Store the diffuse per-fragment color
    gAlbedoSpec.rgb = texture(albedoMap, vTexCoords).rgb;
    // Store specular intensity in alpha component
    gAlbedoSpec.a = texture(specularMap, vTexCoords).r;
}