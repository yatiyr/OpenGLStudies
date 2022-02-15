#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;

out VS_OUT {
    vec3 FragPos;
    vec2 TexCoords;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
    vec3 TangentLightPos[4];    
} vs_out;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform vec3 lightPositions[4];
uniform vec3 camPos;

// I have changed tutorial code a little bit
// First implemented custom sphere builder 
// classes (both radial and icosahedron)
// calculated normals, tangent and bitangents
// normal will be directly equal to the value
// fetched from normal map
void main()
{
    vs_out.TexCoords = aTexCoords;
    vs_out.FragPos = vec3(model * vec4(aPos, 1.0));

    vec3 T = normalize(mat3(model) * aTangent);
    vec3 B = normalize(mat3(model) * aBitangent);
    vec3 N = normalize(mat3(model) * aNormal);

    // Take inverse of TBN to convert other coordinates to
    // tangnent space system
    mat3 TBN = transpose(mat3(T,B,N));

    vs_out.TangentViewPos = TBN * camPos;
    vs_out.TangentFragPos = TBN * vs_out.FragPos;

    for(int i=0; i<4; i++)
    {
        vs_out.TangentLightPos[i] = TBN * lightPositions[i];
    }

    gl_Position = projection * view * vec4(vs_out.FragPos, 1.0);
}