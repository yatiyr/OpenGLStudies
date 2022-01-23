#ifndef __ADVANCED_LIGHTING_H__
#define __ADVANCED_LIGHTING_H__

#include <Lessons/LessonEssentials.h>
#include <Lessons/AdvancedLighting/AdvancedLightingData.h>

class AdvancedLighting
{
public:
    static void initialConf()
    {

        setVerticalFlip(false);


        // global opengl state
        // -------------------
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        AdvancedLightingData::shader = new ShaderProgram("shaders/advancedLighting/advancedLighting.vert",
                                                         "shaders/advancedLighting/advancedLighting.frag");


        // plane VAO
        unsigned int planeVBO;
        glGenVertexArrays(1, &AdvancedLightingData::planeVAO);
        glGenBuffers(1, &planeVBO);
        glBindVertexArray(AdvancedLightingData::planeVAO);
        glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(AdvancedLightingData::planeVertices), AdvancedLightingData::planeVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
        glBindVertexArray(0);

        std::string floorTexturePath = std::string(ROOT_DIR) + "assets/textures/advancedLighting/wood.png";
        AdvancedLightingData::floorTexture = LoadTexture(floorTexturePath.c_str());


        AdvancedLightingData::shader->use();
        AdvancedLightingData::shader->setInt("texture1", 0);

        AdvancedLightingData::lightPos = glm::vec3(0.0f);

    }


    static void draw(Camera *cam)
    {
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        AdvancedLightingData::shader->use();
        glm::mat4 projection = glm::perspective(glm::radians(cam->Zoom), (float)AdvancedLightingData::SCR_WIDTH/(float)AdvancedLightingData::SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = cam->GetViewMatrix();
        AdvancedLightingData::shader->set4Matrix("projection", projection);
        AdvancedLightingData::shader->set4Matrix("view", view);

        // set light uniforms
        AdvancedLightingData::shader->setVec3("viewPos", cam->Position);
        AdvancedLightingData::shader->setVec3("lightPos", AdvancedLightingData::lightPos);
        AdvancedLightingData::shader->setInt("blinn", AdvancedLightingData::blinn);

        // floor
        glBindVertexArray(AdvancedLightingData::planeVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, AdvancedLightingData::floorTexture);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        //std::cout << (AdvancedLightingData::blinn ? "Blinn-Phong" : "Phong") << std::endl;


    }

    static void clean()
    {
        glDeleteVertexArrays(1, &AdvancedLightingData::planeVAO);
        delete AdvancedLightingData::shader;
    }

    static void processInput(GLFWwindow* window)
    {
        if(glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS && !AdvancedLightingData::blinnKeyPressed)
        {
            AdvancedLightingData::blinn = !AdvancedLightingData::blinn;
            AdvancedLightingData::blinnKeyPressed = true;
        }
        if(glfwGetKey(window, GLFW_KEY_B) == GLFW_RELEASE)
        {
            AdvancedLightingData::blinnKeyPressed = false;
        }
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
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
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
};




#endif