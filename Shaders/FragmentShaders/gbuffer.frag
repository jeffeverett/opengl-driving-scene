#version 330 core
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;

in mat3 TBN;
in vec2 TexCoords;
in vec3 FragPos;

uniform sampler2D albedoMap;
uniform sampler2D normalMap;
uniform sampler2D specularMap;


void main()
{    
    // Store the fragment position vector
    gPosition = FragPos;
    // Store the per-fragment normals
    gNormal = TBN*texture(normalMap, TexCoords).rgb;
    // Store the diffuse per-fragment color
    gAlbedoSpec.rgb = texture(albedoMap, TexCoords).rgb;
    // Store specular intensity in alpha component
    gAlbedoSpec.a = texture(specularMap, TexCoords).r;
}