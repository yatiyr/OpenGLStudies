#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;
in vec3 TangentVertices;
in vec3 Bitangents;


// -------------------- DIRECTIONAL LIGHT DEFINITIONS ----------------------------- //
struct DirectionalLight {
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform DirectionalLight directionalLight;

vec3 CalcDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDir, vec3 diffColor, vec3 specColor);
// --------------------------------------------------------------------------------- //


// ---------------------- POINT LIGHT DEFINITIONS ---------------------------------- //
struct PointLight {

    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;    
};
#define POINT_LIGHT_NUMBER 4
uniform PointLight pointLights[POINT_LIGHT_NUMBER];

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 diffColor, vec3 specColor);
// ---------------------------------------------------------------------------------- //

// ------------------------- SPOT LIGHT DEFINITIONS --------------------------------- //
struct SpotLight {
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;    
};

uniform SpotLight spotLight;

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 diffColor, vec3 specColor);
// ------------------------------------------------------------------------------------- //

// ------------------------------ UTILITY FUNCTIONS ------------------------------------ //
vec4 getDiffColor();
vec4 getSpecColor();
// -------------------------------------------------------------------------------------- //


// keep a struct for Material Properties
struct Material {
    sampler2D texture_diffuse1;
    sampler2D texture_specular1;
    float shininess;
};

uniform vec3 viewPos;
uniform Material material;
uniform float time;

void main()
{

    

    // properties
    vec3 norm      = normalize(Normal);
    vec3 viewDir   = normalize(viewPos - FragPos);
    vec4 test = getDiffColor();
    if(test.a < 0.5)
    {
        discard;
    }
    vec3 diffColor = test.rgb;
    
    vec3 specColor = getSpecColor().rgb;

    // calculate colors from directional light
    vec3 result = CalcDirectionalLight(directionalLight, norm, viewDir, diffColor, specColor);

    // iterate point lights and calculate their contribution to the scene
    for(int i=0; i<POINT_LIGHT_NUMBER; i++)
    {
        result += CalcPointLight(pointLights[i], norm, FragPos, viewDir, diffColor, specColor);
    }

    // calculate colors from spot light
    result += CalcSpotLight(spotLight, norm, FragPos, viewDir, diffColor, specColor);

    FragColor = vec4(result, 1.0);

}

// Directional light calculation implementation
vec3 CalcDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDir, vec3 diffColor, vec3 specColor)
{
    vec3 lightDir = normalize(-light.direction);
    
    // diffuse shading
    float dif = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    // sum results
    vec3 ambient  = light.ambient  * diffColor;
    vec3 diffuse  = light.diffuse  * dif  * diffColor;
    vec3 specular = light.specular * spec * specColor;
    return (ambient + diffuse + specular);

}

// Point light calculation implementation
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 diffColor, vec3 specColor)
{
    vec3 lightDir = normalize(light.position - fragPos);

    // diffuse shading
    float dif = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec      = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // attenuation
    float dist        = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * dist + light.quadratic * (dist * dist));

    // sum results
    vec3 ambient  = light.ambient  * diffColor;
    vec3 diffuse  = light.diffuse  * dif  * diffColor;
    vec3 specular = light.specular * spec * specColor;

    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;

    return (ambient + diffuse + specular);
}

// Spot light calculation implementation
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 diffColor, vec3 specColor)
{
    vec3 lightDir = normalize(light.position - FragPos);

    // calculate intensity for spot light
    float theta     = dot(lightDir, normalize(-light.direction));
    float epsilon   = light.cutOff - light.outerCutOff;
    float intensity =  clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);


    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    vec3 ambient = light.ambient * diffColor;
    vec3 diffuse = light.diffuse * diff * diffColor;
    vec3 specular = light.specular * spec * specColor;    

    // attenuation
    float dist = length(light.position - FragPos);
    float attenuation = 1.0 / (light.constant + light.linear * dist + light.quadratic * (dist * dist));

    ambient  *= attenuation;
    diffuse  *= attenuation * intensity;
    specular *= attenuation * intensity;

    // compute frag color here by combining ambient diffuse and specular components
    return (ambient + diffuse + specular);
}


// ----------------------------------- UTILITY FUNCTIONS IMPLEMENTATIONS -------------------------------- //
vec4 getDiffColor()
{
    return vec4(texture(material.texture_diffuse1, TexCoord)); 
}

vec4 getSpecColor()
{
    return vec4(texture(material.texture_specular1, TexCoord));
}
// ------------------------------------------------------------------------------------------------------- //