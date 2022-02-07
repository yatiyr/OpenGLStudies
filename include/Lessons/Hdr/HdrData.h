#ifndef __HDR_DATA_H__
#define __HDR_DATA_H__

#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <ShaderProgram.h>
#include <Model.h>

namespace HdrData
{
    unsigned int SCR_WIDTH = 1920;
    unsigned int SCR_HEIGHT = 1080;

    bool hdr = true;
    bool hdrKeyPressed = false;
    float exposure = 1.0f;

    ShaderProgram *shader;
    ShaderProgram *hdrShader;

    unsigned int woodTexture;

    unsigned int hdrFBO;
    unsigned int colorBuffer;

    unsigned int cubeVAO;
    unsigned int quadVAO;

    std::vector<glm::vec3> lightPositions;
    std::vector<glm::vec3> lightColors;
}



#endif