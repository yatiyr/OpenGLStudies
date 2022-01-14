#ifndef __INSTANCING_DATA_H__
#define __INSTANCING_DATA_H__

#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <ShaderProgram.h>
#include <Model.h>

namespace InstancingData
{
    ShaderProgram *asteroidShader;
    ShaderProgram *planetShader;

    Model *rock;
    Model *planet;

    unsigned int amount = 100000;

    glm::mat4 *modelMatrices;

    unsigned int SCR_WIDTH = 1440;
    unsigned int SCR_HEIGHT = 700;


}

#endif