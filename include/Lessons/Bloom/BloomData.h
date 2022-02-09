#ifndef __BLOOM_DATA_H__
#define __BLOOM_DATA_H__

#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <ShaderProgram.h>
#include <Model.h>

namespace BloomData
{
    unsigned int SCR_WIDTH = 1600;
    unsigned int SCR_HEIGHT = 800;

    bool bloom = true;
    bool bloomKeyPressed = false;
    float exposure = 1.0f;

    ShaderProgram *shader;
    ShaderProgram *shaderLight;
    ShaderProgram *shaderBlur;
    ShaderProgram *shaderBloomFinal;

    unsigned int woodTexture;
    unsigned int containerTexture;

    unsigned int hdrFBO;
    unsigned int colorBuffers[2];

    unsigned int pingpongFBO[2];
    unsigned int pingpongColorbuffers[2];

    std::vector<glm::vec3> lightPositions;
    std::vector<glm::vec3> lightColors;

    unsigned int cubeVAO;
    unsigned int quadVAO;
}


#endif