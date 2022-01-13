#ifndef __GEOMETRY_SHADER_DATA_H__
#define __GEOMETRY_SHADER_DATA_H__

#include <ShaderProgram.h>


namespace GeometryShaderData
{
    Model* backpackModel;

    ShaderProgram* shader;
    ShaderProgram* normalShader;

    unsigned int SCR_WIDTH = 800;
    unsigned int SCR_HEIGHT = 600;

}



#endif