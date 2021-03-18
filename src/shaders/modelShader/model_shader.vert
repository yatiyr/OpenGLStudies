#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec3 aTangentVertices;
layout (location = 4) in vec3 aBitangents;


uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

// it is derived from model matrix
// but calculating it here is costly
// so we calculate it in cpu
uniform mat3 normalMatrix;

// to get real world position of the fragment
out vec3 FragPos;

// sending normal to fragment shader
out vec3 Normal;

// send tex coordinates to fragment shader
out vec2 TexCoord;

// send tangent vertices
out vec3 TangentVertices;

// send bitangents
out vec3 Bitangents;


void main()
{
    FragPos         = vec3(model * vec4(aPos, 1.0));
    Normal          = normalMatrix * aNormal;
    TexCoord        = aTexCoord;
    TangentVertices = aTangentVertices;
    Bitangents      = aBitangents;
    
    gl_Position = projection * view * model * vec4(aPos, 1.0);    
}