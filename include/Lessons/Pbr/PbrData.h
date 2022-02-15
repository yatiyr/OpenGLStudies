#ifndef __PBR_DATA_H__
#define __PBR_DATA_H__

#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <ShaderProgram.h>
#include <Model.h>
#include <Geometry/RadialSphere.h>
#include <Geometry/Icosphere.h>

namespace PbrData
{
    unsigned int SCR_WIDTH = 1920;
    unsigned int SCR_HEIGHT = 1080;

    ShaderProgram *shader;

    RadialSphere *sphere;
    Icosphere *sphere2;

    glm::vec3 lightPositions[] = {
        glm::vec3(-10.0f,  10.0f, 10.0f),
        glm::vec3( 10.0f,  10.0f, 10.0f),
        glm::vec3(-10.0f, -10.0f, 10.0f),
        glm::vec3( 10.0f, -10.0f, 10.0f)        
    };

    glm::vec3 lightColors[] = {
        glm::vec3(300.0f, 300.0f, 300.0f),
        glm::vec3(300.0f, 300.0f, 300.0f),
        glm::vec3(300.0f, 300.0f, 300.0f),
        glm::vec3(300.0f, 300.0f, 300.0f)        
    };

    int nrRows = 7;
    int nrColumns = 7;

    float spacing = 2.5f;

    glm::mat4 projection;

    unsigned int albedoMap;
    unsigned int normalMap;
    unsigned int metallicMap;
    unsigned int roughnessMap;
    unsigned int aoMap;
}


#endif