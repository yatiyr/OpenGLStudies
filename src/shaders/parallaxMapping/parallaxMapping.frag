#version 330 core
out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec2 TexCoords;
    vec3 TangentLightPos;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
} fs_in;

uniform sampler2D diffuseMap;
uniform sampler2D normalMap;
uniform sampler2D heightMap;

uniform float heightScale;

float parallaxShadowMultiplier(in vec3 lightDir, in vec2 initialTexCoord, in float initialHeight, out float layerHeight)
{
    float shadowMultiplier = 1;

    const float minLayers = 32;
    const float maxLayers = 128;

    if(dot(vec3(0.0, 0.0, 1.0), lightDir) > 0)
    {
        float numSamplesUnderSurface = 0;
        shadowMultiplier = 0;
        float numLayers = mix(maxLayers, minLayers, abs(dot(vec3(0.0, 0.0, 1.0), lightDir)));
        layerHeight = initialHeight / numLayers;
        if(layerHeight <= 0.0)
            layerHeight = 0.001;
        vec2 texStep = heightScale * lightDir.xy / lightDir.z / numLayers;

        // current parameters
        float currentLayerHeight = initialHeight - layerHeight;
        vec2 currentTextureCoords = initialTexCoord + texStep;
        float heightFromTexture = 1.0 - texture(heightMap, currentTextureCoords).r;
        int stepIndex = 1;

        // while point is below depth 0.0
        while(currentLayerHeight > 0 && !isinf(currentLayerHeight))
        {
            // if point is under the surface
            if(heightFromTexture < currentLayerHeight)
            {
                // calculate partial shadowing factor
                numSamplesUnderSurface += 1;
                float newShadowMultiplier = (currentLayerHeight - heightFromTexture) * (1.0 - stepIndex / numLayers);
                shadowMultiplier = max(shadowMultiplier, newShadowMultiplier);
            }

            // offset to the next layer
            stepIndex += 1;
            currentLayerHeight -= layerHeight;
            //currentLayerHeight = 0;
            currentTextureCoords += texStep;
            heightFromTexture = 1 - texture(heightMap, currentTextureCoords).r;
            //break;
        }
        

        // Shadowing factor should be 1 if there were no points under the surface
        if(numSamplesUnderSurface < 1)
        {
            shadowMultiplier = 1;
        }
        else
        {
            shadowMultiplier = 1.0 - shadowMultiplier;
        }
    }

    return shadowMultiplier;
}

vec2 ParallaxMapping(in vec2 texCoords, in vec3 viewDir, out float parallaxHeight)
{
    // number of depth layers
    const float minLayers = 32;
    const float maxLayers = 128;
    float numLayers = mix(maxLayers, minLayers, abs(dot(vec3(0.0, 0.0, 1.0), viewDir)));

    // calculate the size of each layer
    float layerDepth = 1.0 / numLayers;
    // depth of current layer
    float currentLayerDepth = 0.0;
    // the amount to shift the texture coordinates per layer (from vector P)
    vec2 P = viewDir.xy / viewDir.z * heightScale;
    vec2 deltaTexCoords = P / numLayers;

    // get initial values
    vec2 currentTexCoords = texCoords;
    float currentDepthMapValue = 1.0 - texture(heightMap, currentTexCoords).r;

    while(currentLayerDepth < currentDepthMapValue)
    {
        // shift texture coordinates along direction of P
        currentTexCoords -= deltaTexCoords;
        // get depthmap value at current texture coordinates
        currentDepthMapValue = 1 - texture(heightMap, currentTexCoords).r;
        // get depth of next layer
        currentLayerDepth += layerDepth;
    }

    // get texture coordinates before collision
    vec2 prevTexCoords = currentTexCoords + deltaTexCoords;

    // get depth after and before collision for linear interpolation
    float afterDepth = currentDepthMapValue - currentLayerDepth;
    float beforeDepth = 1 - texture(heightMap, prevTexCoords).r - currentLayerDepth + layerDepth;

    // interpolation of texture coordinates
    float weight = afterDepth / (afterDepth - beforeDepth);
    vec2 finalTexCoords = prevTexCoords * weight + currentTexCoords * (1.0 - weight);

    // interpolation of depth values
    parallaxHeight = currentLayerDepth + beforeDepth * weight + afterDepth * (1.0 - weight);

    return finalTexCoords;
}


void main()
{
    // offset texture coordinates with Parallax Mapping
    vec3 viewDir = normalize(fs_in.TangentViewPos - fs_in.TangentFragPos);
    vec2 texCoords = fs_in.TexCoords;

    float parallaxHeight;
    texCoords = ParallaxMapping(fs_in.TexCoords, viewDir, parallaxHeight);
    if(texCoords.x > 1.0 || texCoords.y > 1.0 || texCoords.x < 0.0 || texCoords.y < 0.0)
        discard;


    // obtain normal from normal map
    vec3 normal = texture(normalMap, texCoords).rgb;
    normal = normalize(normal * 2.0 - 1.0);

    // diffuse
    vec3 color = texture(diffuseMap, texCoords).rgb;
    // ambient
    vec3 ambient = 0.1 * color;
    // diffuse
    vec3 lightDir = normalize(fs_in.TangentLightPos - fs_in.TangentFragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * color;

    // specular
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 62.0);

    vec3 specular = vec3(0.35) * spec;

    // get self-shadowing factor
    float layerHeight;
    float shadowMultiplier = parallaxShadowMultiplier(lightDir, texCoords, parallaxHeight - 0.02, layerHeight);

    FragColor = vec4(ambient + (diffuse + specular) * pow(shadowMultiplier, 16), 1.0);//vec4(vec3(parallaxHeight), 1.0);
}