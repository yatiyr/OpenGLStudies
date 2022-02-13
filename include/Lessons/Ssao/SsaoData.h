#ifndef __SSAO_DATA_H__
#define __SSAO_DATA_H__

#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <ShaderProgram.h>
#include <Model.h>
#include <Geometry/Sphere.h>

namespace SsaoData
{
    unsigned int SCR_WIDTH = 1600;
    unsigned int SCR_HEIGHT = 800;

    ShaderProgram *shaderGeometryPass;
    ShaderProgram *shaderLightingPass;
    ShaderProgram *shaderSSAO;
    ShaderProgram *shaderSSAOBlur;

    Model *backpack;

    Sphere *sphere;

    unsigned int gBuffer;
    unsigned int gPosition, gNormal, gAlbedo;

    unsigned int ssaoFBO, ssaoBlurFBO;
    unsigned int ssaoColorBuffer, ssaoColorBufferBlur;

    std::vector<glm::vec3> ssaoKernel;
    std::vector<glm::vec3> ssaoNoise;
    unsigned int noiseTexture;

    glm::vec3 lightPos = glm::vec3(2.0f, 4.0f, -2.0f);
    glm::vec3 lightColor = glm::vec3(0.2f, 0.2f, 0.7f);

    unsigned int cubeVAO;
    unsigned int quadVAO;
}



#endif