#ifndef __LESSON_CUBEMAPS_H__
#define __LESSON_CUBEMAPS_H__

#include <Lessons/LessonEssentials.h>
#include <Lessons/Cubemaps/LessonCubemapsData.h>
#include <Model.h>

class LessonCubemaps
{
public:

    static void initialConf()
    {
        // flip loaded texture vertically
        setVerticalFlip(false);

        // Global opengl state
        glEnable(GL_DEPTH_TEST);

        LessonCubemapsData::shader = new ShaderProgram("shaders/cubemap/shader.vert",
                                                       "shaders/cubemap/shader.frag");
        
        LessonCubemapsData::skyboxShader = new ShaderProgram("shaders/cubemap/skybox.vert",
                                                             "shaders/cubemap/skybox.frag");

        std::string backpackPath = ROOT_DIR + std::string("assets/models/backpack/backpack.obj");
        LessonCubemapsData::backpackModel = new Model(backpackPath.c_str());


        // cube VAO
        glGenVertexArrays(1, &LessonCubemapsData::cubeVAO);
        glGenBuffers(1, &LessonCubemapsData::cubeVBO);
        glBindVertexArray(LessonCubemapsData::cubeVAO);
        glBindBuffer(GL_ARRAY_BUFFER, LessonCubemapsData::cubeVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(LessonCubemapsData::cubeVertNormal), &LessonCubemapsData::cubeVertNormal, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));

        // skybox VAO
        glGenVertexArrays(1, &LessonCubemapsData::skyboxVAO);
        glGenBuffers(1, &LessonCubemapsData::skyboxVBO);
        glBindVertexArray(LessonCubemapsData::skyboxVAO);
        glBindBuffer(GL_ARRAY_BUFFER, LessonCubemapsData::skyboxVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(LessonCubemapsData::skyboxVertices), &LessonCubemapsData::skyboxVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

        std::string cubeTexturePath = std::string(ROOT_DIR) + "assets/textures/blend/marble.png";

        //LessonCubemapsData::cubeTexture = LoadTexture(cubeTexturePath.c_str());

        std::vector<std::string> faces
        {
            std::string(ROOT_DIR) + std::string("assets/textures/skybox1/right.jpg"),
            std::string(ROOT_DIR) + std::string("assets/textures/skybox1/left.jpg"),
            std::string(ROOT_DIR) + std::string("assets/textures/skybox1/top.jpg"),
            std::string(ROOT_DIR) + std::string("assets/textures/skybox1/bottom.jpg"),
            std::string(ROOT_DIR) + std::string("assets/textures/skybox1/front.jpg"),
            std::string(ROOT_DIR) + std::string("assets/textures/skybox1/back.jpg")
        };

        LessonCubemapsData::cubemapTexture = LoadSkybox(faces);

        // shader configuration
        LessonCubemapsData::shader->use();
        LessonCubemapsData::shader->setInt("skybox", 0);

        LessonCubemapsData::skyboxShader->use();
        LessonCubemapsData::skyboxShader->setInt("skybox", 0);
    }

    static void draw(Camera *cam)
    {
        // render
        // ------
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // draw scene as normal
        LessonCubemapsData::shader->use();
        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view  = cam->GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(cam->Zoom), (float)LessonCubemapsData::SCR_WIDTH / (float)LessonCubemapsData::SCR_HEIGHT, 0.1f, 100.0f);
        LessonCubemapsData::shader->set4Matrix("model", model);
        LessonCubemapsData::shader->set4Matrix("view", view);
        LessonCubemapsData::shader->set4Matrix("projection", projection);
        LessonCubemapsData::shader->setVec3("cameraPos", cam->Position);

        // cubes
        glBindVertexArray(LessonCubemapsData::cubeVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, LessonCubemapsData::cubemapTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);


        // backpack
        model = glm::translate(model, glm::vec3(5.0f, 5.0f, 5.0f));
        LessonCubemapsData::shader->set4Matrix("model", model);
        LessonCubemapsData::backpackModel->DrawNoTexture(*LessonCubemapsData::shader, LessonCubemapsData::cubemapTexture);

        // draw skybox as last
        glDepthFunc(GL_LEQUAL); // change depth function so depth test passes when values are equal to depth buffer's content
        LessonCubemapsData::skyboxShader->use();
        view = glm::mat4(glm::mat3(cam->GetViewMatrix())); // remove translation from the view matrix
        LessonCubemapsData::skyboxShader->set4Matrix("view", view);
        LessonCubemapsData::skyboxShader->set4Matrix("projection", projection);
        // skybox cube
        glBindVertexArray(LessonCubemapsData::skyboxVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, LessonCubemapsData::cubemapTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glDepthFunc(GL_LESS); // set depth function back to default

    }

    static void clean()
    {

        glDeleteVertexArrays(1, &LessonCubemapsData::cubeVAO);
        glDeleteVertexArrays(1, &LessonCubemapsData::skyboxVAO);
        glDeleteBuffers(1, &LessonCubemapsData::cubeVBO);
        glDeleteBuffers(1, &LessonCubemapsData::skyboxVBO);

        delete LessonCubemapsData::shader;
        delete LessonCubemapsData::skyboxShader;
    }

    static unsigned int LoadTexture(char const* path)
    {
        unsigned int textureID;
        glGenTextures(1, &textureID);

        int width, height, nrComponents;
        unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
        if(data)
        {
            GLenum format;
            if (nrComponents == 1)
                format = GL_RED;
            else if(nrComponents == 3)
                format = GL_RGB;
            else if(nrComponents == 4)
                format = GL_RGBA;

            glBindTexture(GL_TEXTURE_2D, textureID);
            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            stbi_image_free(data);
        }
        else
        {
            std::cout << "Texture failed to load at path: " << path << std::endl;
            stbi_image_free(data);
        }

        return textureID;        
    }

    static unsigned int LoadSkybox(std::vector<std::string> faces)
    {
        unsigned int textureID;
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

        int width, height, nrChannels;
        for(unsigned int i=0; i<faces.size(); i++)
        {
            unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
            if (data)
            {
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
                stbi_image_free(data);
            }
            else
            {
                std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
                stbi_image_free(data);
            }
        }
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        return textureID;
    }    
};



#endif