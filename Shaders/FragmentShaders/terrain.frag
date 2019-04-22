#version 400 core

// Inputs
in vec3 gPosition;
in vec2 gTexCoords;
in vec3 gFacetNormal;
in vec3 gTriDistance;
in vec3 gPatchDistance;

// Outputs
layout (location = 0) out vec3 fPosition;
layout (location = 1) out vec3 fNormal;
layout (location = 2) out vec4 fAlbedoSpec;

// Uniforms
uniform sampler2D albedoMap;
uniform sampler2D normalMap;
uniform sampler2D specularMap;


void main()
{
    // Store the fragment position vector
    fPosition = gPosition;
    // Store the per-fragment normals
    fNormal = mat3(1,0,0,0,0,1,0,1,0)*texture(normalMap, gTexCoords).rgb;
    // Store the diffuse per-fragment color
    fAlbedoSpec.rgb = texture(albedoMap, gTexCoords).rgb;
    // Store specular intensity in alpha component
    fAlbedoSpec.a = texture(specularMap, gTexCoords).r;
}