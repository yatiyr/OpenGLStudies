#ifndef __DEFERRED_SHADING_DATA_H__
#define __DEFERRED_SHADING_DATA_H__

#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <ShaderProgram.h>
#include <Model.h>

namespace DeferredShadingData
{
    unsigned int SCR_WIDTH = 1600;
    unsigned int SCR_HEIGHT = 800;

    ShaderProgram *shaderGeometryPass;
    ShaderProgram *shaderLightingPass;
    ShaderProgram *shaderLightBox;

    Model *backpack;
    std::vector<glm::vec3> objectPositions;

    unsigned int gBuffer;
    unsigned int gPosition, gNormal, gAlbedoSpec;

    unsigned int NR_LIGHTS = 32;
    std::vector<glm::vec3> lightPositions;
    std::vector<glm::vec3> lightColors;
    
    unsigned int cubeVAO;
    unsigned int quadVAO;

}





#endif