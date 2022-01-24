#ifndef __GAMMA_CORRECTION_H__
#define __GAMMA_CORRECTION_H__

#include <Lessons/LessonEssentials.h>
#include <Lessons/GammaCorrection/GammaCorrectionData.h>

class GammaCorrection
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

        // build and compile shaders
        // -------------------------
        GammaCorrectionData::shader = new ShaderProgram("shaders/gammaCorrection/gammaCorrection.vert",
                                                        "shaders/gammaCorrection/gammaCorrection.frag");

        // plane VAO
        unsigned int planeVBO;
        glGenVertexArrays(1, &GammaCorrectionData::planeVAO);
        glGenBuffers(1, &planeVBO);
        glBindVertexArray(GammaCorrectionData::planeVAO);
        glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GammaCorrectionData::planeVertices), GammaCorrectionData::planeVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
        glBindVertexArray(0);

        std::string floorTexturePath = std::string(ROOT_DIR) + "assets/textures/advancedLighting/wood.png";
        GammaCorrectionData::floorTexture = LoadTexture(floorTexturePath.c_str(), false);
        GammaCorrectionData::floorTextureGammaCorrected = LoadTexture(floorTexturePath.c_str(), true);

        // shader configuration
        GammaCorrectionData::shader->use();
        GammaCorrectionData::shader->setInt("floorTexture", 0);
    }

    static void draw(Camera *cam)
    {
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        GammaCorrectionData::shader->use();
        glm::mat4 projection = glm::perspective(glm::radians(cam->Zoom), (float)GammaCorrectionData::SCR_WIDTH / (float)GammaCorrectionData::SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = cam->GetViewMatrix();
        GammaCorrectionData::shader->set4Matrix("projection", projection);
        GammaCorrectionData::shader->set4Matrix("view", view);
        // set light uniforms
        glUniform3fv(glGetUniformLocation(GammaCorrectionData::shader->id, "lightPositions"), 4, &GammaCorrectionData::lightPositions[0][0]);
        glUniform3fv(glGetUniformLocation(GammaCorrectionData::shader->id, "lightColors"), 4, &GammaCorrectionData::lightColors[0][0]);
        GammaCorrectionData::shader->setVec3("viewPos", cam->Position);
        GammaCorrectionData::shader->setInt("gamma", GammaCorrectionData::gammaEnabled);
        // floor
        glBindVertexArray(GammaCorrectionData::planeVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, GammaCorrectionData::gammaEnabled ? GammaCorrectionData::floorTextureGammaCorrected : GammaCorrectionData::floorTexture);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        //std::cout << (GammaCorrectionData::gammaEnabled ? "Gamma Enabled" : "Gamma Disabled") << std::endl;
    }

    static void clean()
    {
        glDeleteVertexArrays(1, &GammaCorrectionData::planeVAO);
        delete GammaCorrectionData::shader;
    }

    static void processInput(GLFWwindow *window)
    {
        if(glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && !GammaCorrectionData::gammaKeyPressed)
        {
            GammaCorrectionData::gammaEnabled = !GammaCorrectionData::gammaEnabled;
            GammaCorrectionData::gammaKeyPressed = true;
        }
        if(glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE)
        {
            GammaCorrectionData::gammaKeyPressed = false;
        }
    }


    static unsigned int LoadTexture(char const* path, bool gammaCorrection)
    {
        unsigned int textureID;
        glGenTextures(1, &textureID);

        int width, height, nrComponents;
        unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
        if (data)
        {
            GLenum internalFormat;
            GLenum dataFormat;
            if (nrComponents == 1)
            {
                internalFormat = dataFormat = GL_RED;
            }
            else if (nrComponents == 3)
            {
                internalFormat = gammaCorrection ? GL_SRGB : GL_RGB;
                dataFormat = GL_RGB;
            }
            else if (nrComponents == 4)
            {
                internalFormat = gammaCorrection ? GL_SRGB_ALPHA : GL_RGBA;
                dataFormat = GL_RGBA;
            }

            glBindTexture(GL_TEXTURE_2D, textureID);
            glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, dataFormat, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); 
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
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