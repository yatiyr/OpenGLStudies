#ifndef __SHADER_PROGRAM__
#define __SHADER_PROGRAM__

#include <iostream>
#include <fstream>
#include <sstream>

#include <glad/glad.h>

class ShaderProgram {
public:
    unsigned int id;

    ShaderProgram(const char* vertexPath, const char* fragmentPath)
    {
        std::string vertexCode;
        std::string fragmentCode;
        std::ifstream vShaderFile;
        std::ifstream fShaderFile;

        vShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
        fShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);

        try
        {
            vShaderFile.open(vertexPath);
            fShaderFile.open(fragmentPath);
            std::stringstream vShaderStream, fShaderStream;

            vShaderStream << vShaderFile.rdbuf();
            fShaderStream << fShaderFile.rdbuf();

            vShaderFile.close();
            fShaderFile.close();

            vertexCode = vShaderStream.str();
            fragmentCode = fShaderStream.str();
        }
        catch(std::ifstream::failure e)
        {
            std::cout << "ERROR:SHADER::FILE_NOT_SUCCESSFULLY_READ" << std::endl;
        }
        const char* vShaderCode = vertexCode.c_str();
        const char* fShaderCode = fragmentCode.c_str();

        unsigned int vertex, fragment;
        int success;
        char infoLog[512];

        // ------------- COMPILE VERTEX SHADER ------------- //
        unsigned int vertexShader;
        vertexShader = glCreateShader(GL_VERTEX_SHADER);

        // Attach the source code to the shader object and compile the shader
        // glShaderSource : param 1 -> shader object to compile
        //                  param 2 -> how many string we're passing as source code
        //                  param 3 -> actual source code of the vertex shader
        //                  param 4 -> NULL (tutorial does not mention what it is :D)
        glShaderSource(vertexShader, 1, &vShaderCode, nullptr);
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
        glShaderSource(fragmentShader, 1, &fShaderCode, nullptr);
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
        id = glCreateProgram();
        glAttachShader(id, vertexShader);
        glAttachShader(id, fragmentShader);
        glLinkProgram(id);

        // check if linking shader program is successful
        int successSP;
        char infoLogSP[512];
        glGetProgramiv(id, GL_LINK_STATUS, &successSP);
        if(!successSP)
        {
            glGetProgramInfoLog(id, 512, nullptr, infoLogSP);
            std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLogSP << '\n';
        }

        // Delete shaders because we don't need them anymore
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);         
    }    

    ~ShaderProgram()
    {
        glDeleteProgram(id);
    }

    void use()
    {
        // We activate shaderProgram by calling glUseProgram
        // Every shader and rendering call after glUseProgram will
        // now use this program object        
        glUseProgram(id);
    }

    void setBool(const std::string &name, bool value) const
    {
        glUniform1i(glGetUniformLocation(id, name.c_str()), (int)value);
    }

    void setInt(const std::string &name, int value) const
    {
        glUniform1i(glGetUniformLocation(id, name.c_str()), value);
    }

    void setFloat(const std::string &name, float value) const 
    {
        glUniform1f(glGetUniformLocation(id, name.c_str()), value);
    }

    void set4Float(const std::string &name, float x, float y, float z, float w)
    {
        glUniform4f(glGetUniformLocation(id,name.c_str()), x, y, z, w);
    }

};


#endif /*__SHADER_PROGRAM__*/