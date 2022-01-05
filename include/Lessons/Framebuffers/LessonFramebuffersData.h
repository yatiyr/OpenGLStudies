#ifndef __LESSON_FRAMEBUFFERS_DATA_H__
#define __LESSON_FRAMEBUFFERS_DATA_H__

#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <ShaderProgram.h>


namespace LessonFramebuffersData
{

    // For framebuffer configuration
    unsigned int framebuffer;
    unsigned int textureColorBuffer;
    unsigned int rbo;

    unsigned int cubeVAO;
    unsigned int cubeVBO;

    unsigned int planeVAO;
    unsigned int planeVBO;

    unsigned int quadVAO;
    unsigned int quadVBO;

    unsigned int cubeTexture;
    unsigned int floorTexture;
    
    ShaderProgram *screenShader;
    ShaderProgram *shader;

    unsigned int SCR_WIDTH = 800;
    unsigned int SCR_HEIGHT = 600;

    static float verticesCube[] = {
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
       -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
       -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
       -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,

        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
       -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
       -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
       -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,

        0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
       -0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
       -0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
       -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,

        0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
       -0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
       -0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
       -0.5f, -0.5f, -0.5f,  1.0f, 1.0f,

       -0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
       -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
       -0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
       -0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
       -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
       -0.5f, -0.5f, -0.5f,  0.0f, 0.0f
    };

    static float planeVertices[] = {
        // positions       // texture Coords
        5.0f, -0.5f,  5.0f, 2.0f, 0.0f,
       -5.0f, -0.5f,  5.0f, 0.0f, 0.0f,
       -5.0f, -0.5f, -5.0f, 0.0f, 2.0f,
        
        5.0f, -0.5f,  5.0f, 2.0f, 0.0f,
       -5.0f, -0.5f, -5.0f, 0.0f, 2.0f,
        5.0f, -0.5f, -5.0f, 2.0f, 2.0f        
    };

    // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
    float quadVertices[] = {
        // positions   // texCoords
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };    

}



#endif