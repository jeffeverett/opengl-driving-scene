#version 400 core

// Inputs
in vec3 vPosition;
in vec2 vTexCoords;
flat in mat3 vTBN;

// Outputs
layout (location = 0) out vec4 fPosition;
layout (location = 1) out vec3 fNormal;
layout (location = 2) out vec4 fAlbedoSpec;

// Uniforms
uniform sampler2D albedoMap;
uniform sampler2D normalMap;
uniform sampler2D specularMap;


void main()
{
    // Store the fragment position vector
    fPosition = vec4(vPosition, 1.0);
    // Store the per-fragment normals
    fNormal = vTBN*texture(normalMap, vTexCoords).rgb;
    // Store the diffuse per-fragment color
    fAlbedoSpec.rgb = texture(albedoMap, vTexCoords).rgb;
    // Store specular intensity in alpha component
    fAlbedoSpec.a = texture(specularMap, vTexCoords).r;
}