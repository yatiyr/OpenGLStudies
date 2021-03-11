#ifndef __SHADER_PROGRAM__
#define __SHADER_PROGRAM__

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

class ShaderProgram {
public:
    unsigned int shaderProgram;

    ~ShaderProgram()
    {
        glDeleteProgram(shaderProgram);
    }

    ShaderProgram(const char* vsSource, const char* fsSource)
    {

        // ------------- COMPILE VERTEX SHADER ------------- //
        unsigned int vertexShader;
        vertexShader = glCreateShader(GL_VERTEX_SHADER);

        // Attach the source code to the shader object and compile the shader
        // glShaderSource : param 1 -> shader object to compile
        //                  param 2 -> how many string we're passing as source code
        //                  param 3 -> actual source code of the vertex shader
        //                  param 4 -> NULL (tutorial does not mention what it is :D)
        glShaderSource(vertexShader, 1, &vsSource, nullptr);
        glCompileShader(vertexShader);

        int successVS;
        char infoLogVS[512];
        glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &successVS);

        if(!successVS)
        {
            glGetShaderInfoLog(vertexShader, 512, nullptr, infoLogVS);
            std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLogVS << '\n';
        }

        // ------------ COMPILE FRAGMENT SHADER ------------ //
        unsigned int fragmentShader;
        fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShader, 1, &fsSource, nullptr);
        glCompileShader(fragmentShader);

        int successFS;
        char infoLogFS[512];
        glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &successFS);

        if(!successFS)
        {
            glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLogFS);
            std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLogFS << '\n';
        } 

        // We need to create a shader program and link it with fragment and vertex shaders
        // that we have created
        shaderProgram = glCreateProgram();
        glAttachShader(shaderProgram, vertexShader);
        glAttachShader(shaderProgram, fragmentShader);
        glLinkProgram(shaderProgram);

        // check if linking shader program is successful
        int successSP;
        char infoLogSP[512];
        glGetProgramiv(shaderProgram, GL_LINK_STATUS, &successSP);
        if(!successSP)
        {
            glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLogSP);
            std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLogSP << '\n';
        }

        // Delete shaders because we don't need them anymore
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);                

    }

    void useProgram()
    {
        // We activate shaderProgram by calling glUseProgram
        // Every shader and rendering call after glUseProgram will
        // now use this program object        
        glUseProgram(shaderProgram);
    }

};


#endif /*__SHADER_PROGRAM__*/