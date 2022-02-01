#ifndef __OMNIDIRECTIONAL_SM_H__
#define __OMNIDIRECTIONAL_SM_H__

#include <Lessons/LessonEssentials.h>
#include <Lessons/OmnidirectionalSM/OmnidirectionalSMData.h>


class OmnidirectionalSM
{
public:

    static void initialConf()
    {
        // configure global opengl state
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);

        // build and compile shaders
        OmnidirectionalSMData::shader = new ShaderProgram("shaders/odshadowMapping/point_shadows.vert",
                                                         "shaders/odshadowMapping/point_shadows.frag");
        OmnidirectionalSMData::simpleDepthShader = new ShaderProgram("shaders/odshadowMapping/point_shadows_depth.vert",
                                                                     "shaders/odshadowMapping/point_shadows_depth.frag",
                                                                     "shaders/odshadowMapping/point_shadows_depth.geom");

        // load wood texture
        std::string woodTexturePath = std::string(ROOT_DIR) + "assets/textures/advancedLighting/wood.png";
        OmnidirectionalSMData::woodTexture = LoadTexture(woodTexturePath.c_str());

        // configure depth map framebuffer object
        glGenFramebuffers(1, &OmnidirectionalSMData::depthMapFBO);
        glGenTextures(1, &OmnidirectionalSMData::depthCubemap);
        glBindTexture(GL_TEXTURE_CUBE_MAP, OmnidirectionalSMData::depthCubemap);
        for(int i=0; i<6; i++)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                         0,
                         GL_DEPTH_COMPONENT,
                         OmnidirectionalSMData::SHADOW_WIDTH,
                         OmnidirectionalSMData::SHADOW_HEIGHT,
                         0,
                         GL_DEPTH_COMPONENT,
                         GL_FLOAT,
                         nullptr);
        }
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        // attach depth texture as FBO's depth buffer
        glBindFramebuffer(GL_FRAMEBUFFER, OmnidirectionalSMData::depthMapFBO);
        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, OmnidirectionalSMData::depthCubemap, 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
        // now that we actually created the framebuffer and added all attachments, we want to check if it is actually complete now
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
                    
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // shader configuration
        OmnidirectionalSMData::shader->use();
        OmnidirectionalSMData::shader->setInt("diffuseTexture", 0);
        OmnidirectionalSMData::shader->setInt("depthMap", 1);

    }

    static void draw(Camera *cam, float currentFrame)
    {
        // render
        // ------
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // 0. create depth cubemap transformation matrices
        // -----------------------------------------------
        glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), (float)OmnidirectionalSMData::SHADOW_WIDTH / (float)OmnidirectionalSMData::SHADOW_HEIGHT, OmnidirectionalSMData::near_plane, OmnidirectionalSMData::far_plane);
        std::vector<glm::mat4> shadowTransforms;
        shadowTransforms.push_back(shadowProj * glm::lookAt(OmnidirectionalSMData::lightPos,
                                                            OmnidirectionalSMData::lightPos + glm::vec3(1.0f, 0.0f, 0.0f),
                                                            glm::vec3(0.0f, -1.0f, 0.0f)));
        shadowTransforms.push_back(shadowProj * glm::lookAt(OmnidirectionalSMData::lightPos,
                                                            OmnidirectionalSMData::lightPos + glm::vec3(-1.0f, 0.0f, 0.0f),
                                                            glm::vec3(0.0f, -1.0f, 0.0f)));
        shadowTransforms.push_back(shadowProj * glm::lookAt(OmnidirectionalSMData::lightPos,
                                                            OmnidirectionalSMData::lightPos + glm::vec3(0.0f, 1.0f, 0.0f),
                                                            glm::vec3(0.0f, 0.0f, 1.0f)));
        shadowTransforms.push_back(shadowProj * glm::lookAt(OmnidirectionalSMData::lightPos,
                                                            OmnidirectionalSMData::lightPos + glm::vec3(0.0f, -1.0f, 0.0f),
                                                            glm::vec3(0.0f, 0.0f, -1.0f)));
        shadowTransforms.push_back(shadowProj * glm::lookAt(OmnidirectionalSMData::lightPos,
                                                            OmnidirectionalSMData::lightPos + glm::vec3(0.0f, 0.0f, 1.0f),
                                                            glm::vec3(0.0f, -1.0f, 0.0f)));                                                            
        shadowTransforms.push_back(shadowProj * glm::lookAt(OmnidirectionalSMData::lightPos,
                                                            OmnidirectionalSMData::lightPos + glm::vec3(0.0f, 0.0f, -1.0f),
                                                            glm::vec3(0.0f, -1.0f, 0.0f)));


        // 1. render scene to depth cubemap
        // --------------------------------
        glViewport(0, 0, OmnidirectionalSMData::SHADOW_WIDTH, OmnidirectionalSMData::SHADOW_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, OmnidirectionalSMData::depthMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);
        OmnidirectionalSMData::simpleDepthShader->use();
        for(unsigned int i=0; i<6; i++)
            OmnidirectionalSMData::simpleDepthShader->set4Matrix("shadowMatrices[" + std::to_string(i) + "]", shadowTransforms[i]);
        OmnidirectionalSMData::simpleDepthShader->setFloat("far_plane", OmnidirectionalSMData::far_plane);
        OmnidirectionalSMData::simpleDepthShader->setVec3("lightPos", OmnidirectionalSMData::lightPos);
        renderScene(OmnidirectionalSMData::simpleDepthShader, currentFrame);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // 2. render scene as normal
        // -------------------------
        glViewport(0, 0, OmnidirectionalSMData::SCR_WIDTH, OmnidirectionalSMData::SCR_HEIGHT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        OmnidirectionalSMData::shader->use();
        glm::mat4 projection = glm::perspective(glm::radians(cam->Zoom), (float)OmnidirectionalSMData::SCR_WIDTH/(float)OmnidirectionalSMData::SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = cam->GetViewMatrix();
        OmnidirectionalSMData::shader->set4Matrix("projection", projection);
        OmnidirectionalSMData::shader->set4Matrix("view", view);
        // set lighting uniforms
        OmnidirectionalSMData::shader->setVec3("lightPos", OmnidirectionalSMData::lightPos);
        OmnidirectionalSMData::shader->setVec3("viewPos", cam->Position);
        OmnidirectionalSMData::shader->setInt("shadows", OmnidirectionalSMData::shadows);
        OmnidirectionalSMData::shader->setFloat("far_plane", OmnidirectionalSMData::far_plane);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, OmnidirectionalSMData::woodTexture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_CUBE_MAP, OmnidirectionalSMData::depthCubemap);
        renderScene(OmnidirectionalSMData::shader, currentFrame);
    }

    static void clean()
    {
        glDeleteVertexArrays(1, &OmnidirectionalSMData::cubeVAO);

        delete OmnidirectionalSMData::shader;
        delete OmnidirectionalSMData::simpleDepthShader;
    }

    static void processInput(GLFWwindow* window)
    {
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && !OmnidirectionalSMData::shadowsKeyPressed)
        {
            OmnidirectionalSMData::shadows = !OmnidirectionalSMData::shadows;
            OmnidirectionalSMData::shadowsKeyPressed = true;
        }
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE)
        {
            OmnidirectionalSMData::shadowsKeyPressed = false;
        }
    }    

    static void renderScene(ShaderProgram *shader, float currentFrame)
    {
        // room cube
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::scale(model, glm::vec3(5.0f));
        shader->set4Matrix("model", model);
        glDisable(GL_CULL_FACE);
        OmnidirectionalSMData::shader->setInt("reverse_normals", 1);
        renderCube();
        shader->setInt("reverse_normals", 0);
        glEnable(GL_CULL_FACE);

        // cubes
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(4.0f, -3.5f, 0.0f));
        model = glm::scale(model, glm::vec3(0.5f));
        shader->set4Matrix("model", model);
        renderCube();
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(2.0f, 3.0f, 1.0f));
        model = glm::scale(model, glm::vec3(0.75f));
        shader->set4Matrix("model", model);
        renderCube();
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-3.0f, -1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.5f));
        shader->set4Matrix("model", model);
        renderCube();
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-1.5f, 1.0f, 1.5f));
        model = glm::scale(model, glm::vec3(0.5f));
        shader->set4Matrix("model", model);
        renderCube();
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-1.5f, 2.0f, -3.0f));
        model = glm::rotate(model, glm::radians(60.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
        model = glm::scale(model, glm::vec3(0.75f));
        shader->set4Matrix("model", model);
        renderCube();                                
    }

    static void renderCube()
    {
        if(OmnidirectionalSMData::cubeVAO == 0)
        {
            unsigned int cubeVBO;
            glGenVertexArrays(1, &OmnidirectionalSMData::cubeVAO);
            glGenBuffers(1, &cubeVBO);
            glBindVertexArray(OmnidirectionalSMData::cubeVAO);
            glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(OmnidirectionalSMData::cubeVertices), OmnidirectionalSMData::cubeVertices, GL_STATIC_DRAW);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
            glEnableVertexAttribArray(2);
            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindVertexArray(0);
        }

        glBindVertexArray(OmnidirectionalSMData::cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
    }

    static unsigned int LoadTexture(char const* path)
    {
        unsigned int textureID;
        glGenTextures(1, &textureID);

        int width, height, nrComponents;
        unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
        if (data)
        {
            GLenum format;
            if (nrComponents == 1)
                format = GL_RED;
            else if (nrComponents == 3)
                format = GL_RGB;
            else if (nrComponents == 4)
                format = GL_RGBA;

            glBindTexture(GL_TEXTURE_2D, textureID);
            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT); // for this tutorial: use GL_CLAMP_TO_EDGE to prevent semi-transparent borders. Due to interpolation it takes texels from next repeat 
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