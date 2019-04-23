#version 400 core

// Inputs
in float gTessLevel;
in vec3 gPosition;
in vec2 gTexCoords;
in vec3 gFacetNormal;
in vec3 gTriDistance;
in vec3 gPatchDistance;

// Outputs
layout (location = 0) out vec4 fPosition;
layout (location = 1) out vec3 fNormal;
layout (location = 2) out vec4 fAlbedoSpec;

// Uniforms
uniform int wireframeMode;

uniform sampler2D albedoMap;
uniform sampler2D normalMap;
uniform sampler2D specularMap;

uniform float textureRepeatX;
uniform float textureRepeatZ;


vec3 determineWireframeColor()
{
    // Discretize output
    if (gTessLevel > 30) {
        return vec3(1,0,0);
    }
    if (gTessLevel > 20) {
        return vec3(0,1,0);
    }
    if (gTessLevel > 10) {
        return vec3(1,1,0);
    }
    return vec3(0,0,1);
}

void main()
{
    // Store the fragment position vector
    fPosition = vec4(gPosition, 1.0);
    // Store the per-fragment normals
    fNormal = mat3(1,0,0,0,0,1,0,1,0)*texture(normalMap, gTexCoords).rgb;
    // Store the diffuse per-fragment color
    vec3 color = fAlbedoSpec.rgb = texture(albedoMap, gTexCoords*vec2(textureRepeatX, textureRepeatZ)).rgb;
    vec3 wireframeColor = determineWireframeColor();
    float d = min(min(gTriDistance.x, gTriDistance.y), gTriDistance.z);
    float stepVal = step(0.1, d);
    bool dUnderThreshold = stepVal == 0.0;
    if (wireframeMode == 0) {
        // Output albedo
        fAlbedoSpec.rgb = color;
    }
    else if (wireframeMode == 1) {
        // Output albedo + wireframe
        fAlbedoSpec.rgb = mix(wireframeColor, color, stepVal);
    }
    else {
        // Output wireframe
        if (dUnderThreshold) {
            fAlbedoSpec.rgb = wireframeColor;
        }
        else {
            discard;
        }
    }
    // Store specular intensity in alpha component
    fAlbedoSpec.a = texture(specularMap, gTexCoords).r;
}