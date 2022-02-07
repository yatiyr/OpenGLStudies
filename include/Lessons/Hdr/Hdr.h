#ifndef __HDR_H__
#define __HDR_H__

#include <Lessons/LessonEssentials.h>
#include <Lessons/Hdr/HdrData.h>

class Hdr
{
public:
    static void initialConf()
    {
        // configure global opengl state
        // -----------------------------
        glEnable(GL_DEPTH_TEST);

        // build and compile shaders
        // -------------------------
        HdrData::shader = new ShaderProgram("shaders/hdr/lighting.vert",
                                            "shaders/hdr/lighting.frag");

        HdrData::hdrShader = new ShaderProgram("shaders/hdr/hdr.vert",
                                               "shaders/hdr/hdr.frag");


        std::string woodTexturePath = std::string(ROOT_DIR) + "assets/textures/advancedLighting/wood.png";

        HdrData::woodTexture = LoadTexture(woodTexturePath.c_str(), true);

        // configure floating point framebuffer
        // ------------------------------------
        glGenFramebuffers(1, &HdrData::hdrFBO);
        // create floating point color buffer
        glGenTextures(1, &HdrData::colorBuffer);
        glBindTexture(GL_TEXTURE_2D, HdrData::colorBuffer);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, HdrData::SCR_WIDTH, HdrData::SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // create depth buffer
        unsigned int rboDepth;
        glGenRenderbuffers(1, &rboDepth);
        glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, HdrData::SCR_WIDTH, HdrData::SCR_HEIGHT);
        // attach buffers
        glBindFramebuffer(GL_FRAMEBUFFER, HdrData::hdrFBO);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, HdrData::colorBuffer, 0);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "Framebuffer not complete!" << std::endl;
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // light info
        HdrData::lightPositions.push_back(glm::vec3( 0.0f,  0.0f, 49.5f));
        HdrData::lightPositions.push_back(glm::vec3(-1.4f, -1.9f, 9.0f));
        HdrData::lightPositions.push_back(glm::vec3( 0.0f, -1.8f, 4.0f));
        HdrData::lightPositions.push_back(glm::vec3( 0.8f, -1.7f, 6.0f));

        HdrData::lightColors.push_back(glm::vec3(200.0f, 200.0f, 200.0f));
        HdrData::lightColors.push_back(glm::vec3(0.1f, 0.0f, 0.0f));
        HdrData::lightColors.push_back(glm::vec3(0.0f, 0.0f, 0.2f));
        HdrData::lightColors.push_back(glm::vec3(0.0f, 0.1f, 0.0f));

        HdrData::shader->use();
        HdrData::shader->setInt("diffuseTexture", 0);
        HdrData::hdrShader->use();
        HdrData::hdrShader->setInt("hdrBuffer", 0);
    }

    static void draw(Camera *cam, float currentFrame)
    {
        // render
        // ------
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // 1. render scene into floating point framebuffer
        // -----------------------------------------------
        glBindFramebuffer(GL_FRAMEBUFFER, HdrData::hdrFBO);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glm::mat4 projection = glm::perspective(glm::radians(cam->Zoom), (float)HdrData::SCR_WIDTH / (float)HdrData::SCR_HEIGHT, 0.1f, 100.0f);
            glm::mat4 view = cam->GetViewMatrix();
            HdrData::shader->use();
            HdrData::shader->set4Matrix("projection", projection);
            HdrData::shader->set4Matrix("view", view);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, HdrData::woodTexture);
            // set lighting uniforms
            for (unsigned int i=0; i<HdrData::lightPositions.size(); i++)
            {
                HdrData::shader->setVec3("lights[" + std::to_string(i) + "].Position", HdrData::lightPositions[i]);
                HdrData::shader->setVec3("lights[" + std::to_string(i) + "].Color", HdrData::lightColors[i]);
            }

            HdrData::shader->setVec3("viewPos", cam->Position);
            // render tunnel
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(0.0f, 0.0f, 25.0f));
            model = glm::scale(model, glm::vec3(2.5f, 2.5f, 27.5f));
            HdrData::shader->set4Matrix("model", model);
            HdrData::shader->setInt("inverse_normals", true);
            renderCube();
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // 2. now render floating point color buffer to 2D quad and tonemap HDR colors to default framebuffer's (clamped) color range
        // --------------------------------------------------------------------------------------------------------------------------
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        HdrData::hdrShader->use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, HdrData::colorBuffer);
        HdrData::hdrShader->setInt("hdr", HdrData::hdr);
        HdrData::hdrShader->setFloat("exposure", HdrData::exposure);
        renderQuad();

    }

    static void clean()
    {
        glDeleteVertexArrays(1, &HdrData::cubeVAO);
        glDeleteVertexArrays(1, &HdrData::quadVAO);

        delete HdrData::shader;
        delete HdrData::hdrShader;
    }

    static void renderCube()
    {

        if(HdrData::cubeVAO == 0)
        {
            float vertices[] = {
                // back face
                -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
                 1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
                 1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
                 1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
                -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
                -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
                // front face
                -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
                 1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
                 1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
                 1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
                -1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
                -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
                // left face
                -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
                -1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
                -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
                -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
                -1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
                -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
                // right face
                 1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
                 1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
                 1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right         
                 1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
                 1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
                 1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left     
                // bottom face
                -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
                 1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
                 1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
                 1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
                -1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
                -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
                // top face
                -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
                 1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
                 1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right     
                 1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
                -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
                -1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left        
            };

            unsigned int cubeVBO;
            glGenVertexArrays(1, &HdrData::cubeVAO);
            glGenBuffers(1, &cubeVBO);
            // fill buffer
            glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);
            glBindVertexArray(HdrData::cubeVAO);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
            glEnableVertexAttribArray(2);
            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindVertexArray(0);
        }

        glBindVertexArray(HdrData::cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);

    }

    static void renderQuad()
    {
        if(HdrData::quadVAO == 0)
        {

            float quadVertices[] = {
                // positions        // texture Coords
                -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
                -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
                 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
                 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
            };

            // setup plane VAO
            unsigned int quadVBO;            
            glGenVertexArrays(1, &HdrData::quadVAO);
            glGenBuffers(1, &quadVBO);
            glBindVertexArray(HdrData::quadVAO);
            glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        }

        glBindVertexArray(HdrData::quadVAO);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindVertexArray(0);
    }

    static void processInput(GLFWwindow* window)
    {
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && !HdrData::hdrKeyPressed)
        {
            HdrData::hdr = !HdrData::hdr;
            HdrData::hdrKeyPressed = true;
        }
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE)
        {
            HdrData::hdrKeyPressed = false;
        }

        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        {
            if (HdrData::exposure > 0.0f)
                HdrData::exposure -= 0.001f;
            else
                HdrData::exposure = 0.0f;
        }
        else if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        {
            HdrData::exposure += 0.001f;
        }
    }

    static unsigned int LoadTexture(char const *path, bool gammaCorrection)
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