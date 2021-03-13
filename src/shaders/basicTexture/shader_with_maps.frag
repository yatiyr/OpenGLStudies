#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

// keep a struct for Material Properties
struct Material {
    sampler2D diffuse;
    sampler2D specular;
    sampler2D emission;
    float shininess;
};

// keep a struct for light properties
struct Light {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform vec3 viewPos;
uniform Material material;
uniform Light light;

uniform float time;

void main()
{

    // --------------- Simulate Ambient Light --------------- //
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoord));
    // ------------------------------------------------------ //


    // ---------------- Simulate Diffuse Light --------------- //
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);

    // find cosine between norm and lightDir
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoord));
    // ------------------------------------------------------ //


    // -------------- Simulate Specular Light --------------- //
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);

    // power is shininess
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoord));
    // ------------------------------------------------------ //


    // -------------- Simulate Emission -------------------- //
    vec3 emission = vec3(0.0);
    vec3 specularMapColor = vec3(texture(material.specular, TexCoord));
    if(specularMapColor == vec3(0.0))
    {
        emission = vec3(texture(material.emission, TexCoord));
        emission = emission * max(0.0,(sin(time * 13.5)));
    }

    // ----------------------------------------------------- //

    // compute frag color here by combining ambient diffuse and specular components
    vec3 result = ambient + diffuse + specular + emission;
    FragColor = vec4(result, 1.0);
}