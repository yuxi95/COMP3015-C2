#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTanget;
layout (location = 4) in vec3 aBinormal;

out vec2 TexCoords;
out vec3 WorldPos;
out vec3 Normal;
out vec3 Tanget;
out vec3 Binormal;
out vec4 lsPos;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform mat3 normalMatrix;
uniform mat4 lightSpaceMatrix;

void main()
{
    TexCoords = aTexCoords;
    WorldPos = vec3(model * vec4(aPos, 1.0));
    Normal = normalMatrix * aNormal;   
    Tanget = normalMatrix * aTanget;   
    Binormal = normalMatrix * aBinormal;   

    lsPos = lightSpaceMatrix * vec4( aPos.xyz, 1.0);

    gl_Position =  projection * view * vec4(WorldPos, 1.0);
}