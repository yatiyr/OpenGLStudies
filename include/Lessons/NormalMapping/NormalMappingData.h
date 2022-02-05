#ifndef __NORMAL_MAPPING_DATA_H__
#define __NORMAL_MAPPING_DATA_H__

#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <ShaderProgram.h>
#include <Model.h>

namespace NormalMappingData
{
    unsigned int SCR_WIDTH = 800;
    unsigned int SCR_HEIGHT = 600;

    ShaderProgram *shader;

    unsigned int diffuseMap;
    unsigned int normalMap;

    glm::vec3 lightPos(0.5f, 1.0f, 0.3f); 

    unsigned int quadVAO = 0; 
    unsigned int quadVBO;
    
}



#endif