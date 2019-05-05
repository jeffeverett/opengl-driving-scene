#version 440 core

// Inputs
in vec2 vTexCoords;

// Outputs
out vec4 fFragColor;

// Uniforms
struct DirLight {
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    vec3 position;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct SpotLight {
    vec3  position;
    vec3  direction;
    float innerCutoff;
    float outerCutoff;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct GBufferInputs {
    vec3 position;
    vec3 normal;
    vec3 albedo;
    float specular;
};

// gBuffer textures
uniform sampler2D positionTexture;
uniform sampler2D normalTexture;
uniform sampler2D albedoSpecTexture;

// Lights
uniform DirLight dirLight;
#define NR_POINT_LIGHTS 6
uniform PointLight pointLights[NR_POINT_LIGHTS];
#define NR_SPOT_LIGHTS 4
uniform SpotLight spotLights[NR_SPOT_LIGHTS];

// Other inputs
uniform vec3 viewPos;
uniform vec4 fogColor;
uniform float fogDensity;

vec3 CalcDirLight(DirLight light, vec3 viewDir, GBufferInputs gBufferInputs);
vec3 CalcPointLight(PointLight light, vec3 viewDir, GBufferInputs gBufferInputs);
vec3 CalcSpotLight(SpotLight light, vec3 viewDir, GBufferInputs gBufferInputs);

void main()
{
    // Do nothing if this location was not written to during gBuffer creation
    if (texture(positionTexture, vTexCoords).a == 0.0) {
        discard;
    }

    // Determine gBuffer inputs from textures
    GBufferInputs gBufferInputs;
    gBufferInputs.position = texture(positionTexture, vTexCoords).rgb;
    gBufferInputs.normal = texture(normalTexture, vTexCoords).rgb;
    gBufferInputs.albedo = texture(albedoSpecTexture, vTexCoords).rgb;
    gBufferInputs.specular = texture(albedoSpecTexture, vTexCoords).a;

    // Convencence calculations
    vec3 viewDir = normalize(viewPos - gBufferInputs.position);

    // Phase 1: Directional lighting
    vec3 result = CalcDirLight(dirLight, viewDir, gBufferInputs);
    // Phase 2: Point lights
    for(int i = 0; i < NR_POINT_LIGHTS; i++)
        result += CalcPointLight(pointLights[i], viewDir, gBufferInputs);
    // Phase 3: Spot light
    for (int i = 0; i < NR_SPOT_LIGHTS; i++)
        result += CalcSpotLight(spotLights[i], viewDir, gBufferInputs);
    
    vec4 objectColor = vec4(result, 1.0);
    fFragColor = objectColor;

    /*
    // Phase 4: Apply fog
    float distance = length(viewPos - gBufferInputs.position);
    // Note: same as GL_EXP
    float f = exp(-fogDensity*distance);

    fFragColor = f*objectColor + (1-f)*fogColor;
    */
}

vec3 CalcDirLight(DirLight light, vec3 viewDir, GBufferInputs gBufferInputs)
{
    vec3 lightDir = normalize(-light.direction);
    // Diffuse shading
    float diff = max(dot(gBufferInputs.normal, lightDir), 0.0);
    // Specular shading
    vec3 reflectDir = reflect(-lightDir, gBufferInputs.normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
    // Combine results
    vec3 ambient  = light.ambient  * gBufferInputs.albedo;
    vec3 diffuse  = light.diffuse  * diff * gBufferInputs.albedo;
    vec3 specular = light.specular * spec * vec3(gBufferInputs.specular);
    return (ambient + diffuse + specular);
}

vec3 CalcPointLight(PointLight light, vec3 viewDir, GBufferInputs gBufferInputs)
{
    vec3 lightDir = normalize(light.position - gBufferInputs.position);
    // Diffuse shading
    float diff = max(dot(gBufferInputs.normal, lightDir), 0.0);
    // Specular shading
    vec3 reflectDir = reflect(-lightDir, gBufferInputs.normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
    // Attenuation
    float distance    = length(light.position - gBufferInputs.position);
    float attenuation = 1.0 / (light.constant + light.linear * distance +
  			     light.quadratic * (distance * distance));
    // Combine results
    vec3 ambient  = light.ambient  * gBufferInputs.albedo;
    vec3 diffuse  = light.diffuse  * diff * gBufferInputs.albedo;
    vec3 specular = light.specular * spec * vec3(gBufferInputs.specular);
    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}

vec3 CalcSpotLight(SpotLight light, vec3 viewDir, GBufferInputs gBufferInputs)
{
    vec3 lightDir = normalize(light.position - gBufferInputs.position);
    // Diffuse shading
    float diff = max(dot(gBufferInputs.normal, lightDir), 0.0);
    // Specular shading
    vec3 reflectDir = reflect(-lightDir, gBufferInputs.normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
    // Attenuation
    float distance = length(light.position - gBufferInputs.position);
    float attenuation = min(1, 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance)));
    // Spotlight intensity
    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = light.innerCutoff - light.outerCutoff;
    float intensity = clamp((theta - light.outerCutoff) / epsilon, 0.0, 1.0);
    // Combine results
    vec3 ambient = light.ambient * gBufferInputs.albedo;
    vec3 diffuse = light.diffuse * diff * gBufferInputs.albedo;
    vec3 specular = light.specular * spec * vec3(gBufferInputs.specular);
    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;
    return (ambient + diffuse + specular);
}