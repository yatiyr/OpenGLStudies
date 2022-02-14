#ifndef __PARALLAX_MAPPING_DATA_H__
#define __PARALLAX_MAPPING_DATA_H__

#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <ShaderProgram.h>
#include <Model.h>
#include <Geometry/Icosphere.h>

namespace ParallaxMappingData
{
    unsigned int SCR_WIDTH = 1920;
    unsigned int SCR_HEIGHT = 1080;

    ShaderProgram *shader;

    Icosphere *sphere;

    unsigned int diffuseMap;
    unsigned int normalMap;
    unsigned int heightMap;

    glm::vec3 lightPos(0.5f, 1.0f, 0.3f);

    unsigned int quadVAO = 0;
    unsigned int quadVBO;

    float heightScale = 0.1f;
}

#endif