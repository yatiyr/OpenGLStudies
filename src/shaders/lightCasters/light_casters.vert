#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

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


void main()
{
    FragPos     = vec3(model * vec4(aPos, 1.0));
    Normal      = normalMatrix * aNormal;
    TexCoord   = aTexCoord;
    gl_Position = projection * view * model * vec4(aPos, 1.0);    
}