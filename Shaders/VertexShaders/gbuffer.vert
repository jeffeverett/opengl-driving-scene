#version 440 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;
layout (location = 5) in mat4 aInstanceModel;

out vec3 vPosition;
out vec2 vTexCoords;
out mat3 vTBN;

uniform mat4 view;
uniform mat4 projection;

void main()
{
    vec4 worldPos = aInstanceModel * vec4(aPos, 1.0);
    vPosition = worldPos.xyz; 
    vTexCoords = aTexCoords;

    mat3 normalMatrix = transpose(inverse(mat3(aInstanceModel)));
    vec3 T = normalize(normalMatrix * aTangent);
    vec3 B = normalize(normalMatrix * aBitangent);
    vec3 N = normalize(normalMatrix * aNormal);
    vTBN = mat3(T, B, N);

    gl_Position = projection * view * worldPos;
}