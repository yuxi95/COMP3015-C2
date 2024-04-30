#version 430 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 vertexPosition_modelspace;

//layout (location = 2) in vec2 aTexCoords;
//out vec2 TexCoords;

// Values that stay constant for the whole mesh.
uniform mat4 lightSpaceMatrix;

void main(){
    //TexCoords = aTexCoords;
    
    gl_Position =  lightSpaceMatrix * vec4(vertexPosition_modelspace,1);
}