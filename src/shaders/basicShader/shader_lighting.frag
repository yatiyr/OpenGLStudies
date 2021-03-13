#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;

uniform vec3 viewPos;

// keep a struct for Material Properties
struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

// keep a struct for light properties
struct Light {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform Material material;
uniform Light light;

void main()
{

    // --------------- Simulate Ambient Light --------------- //
    vec3 ambient = light.ambient * material.ambient;
    // ------------------------------------------------------ //


    // ---------------- Simulate Diffuse Light --------------- //
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);

    // find cosine between norm and lightDir
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * (diff * material.diffuse);
    // ------------------------------------------------------ //


    // -------------- Simulate Specular Light --------------- //
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);

    // power is shininess
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * (spec * material.specular);
    // ------------------------------------------------------ //

    // compute frag color here by combining ambient diffuse and specular components
    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0);
}