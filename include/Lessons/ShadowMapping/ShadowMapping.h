#ifndef __SHADOW_MAPPING_H__
#define __SHADOW_MAPPING_H__

#include <Lessons/LessonEssentials.h>
#include <Lessons/ShadowMapping/ShadowMappingData.h>

class ShadowMapping
{
public:
    static void initialConf()
    {
        setVerticalFlip(false);        
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);

        ShadowMappingData::shader = new ShaderProgram("shaders/shadowMapping/shadowmapping.vert",
                                                      "shaders/shadowMapping/shadowmapping.frag");

        ShadowMappingData::simpleDepthShader = new ShaderProgram("shaders/shadowMapping/shadowmapping_depth.vert",
                                                                 "shaders/shadowMapping/shadowmapping_depth.frag");
        
        ShadowMappingData::debugDepthQuad = new ShaderProgram("shaders/shadowMapping/debug_quad.vert",
                                                              "shaders/shadowMapping/debug_quad.frag");

        // plane VAO
        unsigned int planeVBO;
        glGenVertexArrays(1, &ShadowMappingData::planeVAO);
        glGenBuffers(1, &planeVBO);
        glBindVertexArray(ShadowMappingData::planeVAO);
        glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(ShadowMappingData::planeVertices), ShadowMappingData::planeVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
        glBindVertexArray(0);

        std::string woodTexturePath = std::string(ROOT_DIR) + "assets/textures/advancedLighting/wood.png";
        ShadowMappingData::woodTexture = LoadTexture(woodTexturePath.c_str());

        // configure depth map framebuffer object
        glGenFramebuffers(1, &ShadowMappingData::depthMapFBO);
        glGenTextures(1, &ShadowMappingData::depthMap);
        glBindTexture(GL_TEXTURE_2D, ShadowMappingData::depthMap);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, ShadowMappingData::SHADOW_WIDTH, ShadowMappingData::SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);

        // float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        // glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);         
        // attach depth texture as FBO's depth buffer
        glBindFramebuffer(GL_FRAMEBUFFER, ShadowMappingData::depthMapFBO);
        //glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, ShadowMappingData::depthMap, 0);
        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, ShadowMappingData::depthMap, 0);        
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
        // now that we actually created the framebuffer and added all attachments, we want to check if it is actually complete now
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;        
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // configure shaders
        ShadowMappingData::shader->use();
        ShadowMappingData::shader->setInt("diffuseTexture", 0);
        ShadowMappingData::shader->setInt("shadowMap", 1);
        ShadowMappingData::debugDepthQuad->use();
        ShadowMappingData::debugDepthQuad->setInt("depthMap", 0);

    }

    static void draw(Camera *cam, float currentFrame)
    {
        // render
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        //ShadowMappingData::lightPos = glm::vec3(-2.0f  + 1 * std::cos(currentFrame) , 4.0f + 1 * std::sin(currentFrame), -1.0f);

        // 1. render depth of scene to texture (from light's perspective)
        // --------------------------------------------------------------
        glm::mat4 lightProjection, lightView;
        glm::mat4 lightSpaceMatrix;
        float near_plane = 0.2f, far_plane = 7.5f;
        lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
        lightView = glm::lookAt(ShadowMappingData::lightPos, glm::vec3(0.0f), glm::vec3(0.0,1.0,0.0));
        lightSpaceMatrix = lightProjection * lightView;
        // render scene from light's point of view
        ShadowMappingData::simpleDepthShader->use();
        ShadowMappingData::simpleDepthShader->set4Matrix("lightSpaceMatrix", lightSpaceMatrix);
        glViewport(0, 0, ShadowMappingData::SHADOW_WIDTH, ShadowMappingData::SHADOW_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, ShadowMappingData::depthMapFBO);
            glClear(GL_DEPTH_BUFFER_BIT);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, ShadowMappingData::woodTexture);           
            glCullFace(GL_FRONT);
            renderScene(ShadowMappingData::simpleDepthShader, currentFrame);          
            glCullFace(GL_BACK);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // reset viewport
        glViewport(0, 0, ShadowMappingData::SCR_WIDTH, ShadowMappingData::SCR_HEIGHT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // 2. render scene as normal using the generated depth/shadow map
        // --------------------------------------------------------------
        ShadowMappingData::shader->use();
        glm::mat4 projection = glm::perspective(glm::radians(cam->Zoom), (float)ShadowMappingData::SCR_WIDTH / (float)ShadowMappingData::SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = cam->GetViewMatrix();
        ShadowMappingData::shader->set4Matrix("projection", projection);
        ShadowMappingData::shader->set4Matrix("view", view);
        // set light uniforms
        ShadowMappingData::shader->setVec3("viewPos", cam->Position);
        ShadowMappingData::shader->setVec3("lightPos", ShadowMappingData::lightPos);
        glm::mat4 biasMatrix(
            0.5, 0.0, 0.0, 0.0,
            0.0, 0.5, 0.0, 0.0,
            0.0, 0.0, 0.5, 0.0,
            0.5, 0.5, 0.5, 1.0            
        );
        //lightSpaceMatrix = biasMatrix * lightSpaceMatrix;
        ShadowMappingData::shader->set4Matrix("lightSpaceMatrix", lightSpaceMatrix);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, ShadowMappingData::woodTexture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, ShadowMappingData::depthMap);
        renderScene(ShadowMappingData::shader, currentFrame);


        // render Depth map to quad for visual debugging
        ShadowMappingData::debugDepthQuad->use();
        ShadowMappingData::debugDepthQuad->setFloat("near_plane", near_plane);
        ShadowMappingData::debugDepthQuad->setFloat("far_plane", far_plane);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, ShadowMappingData::depthMap);
        renderQuad();
    }

    static void clean()
    {
        glDeleteVertexArrays(1, &ShadowMappingData::quadVAO);
        glDeleteVertexArrays(1, &ShadowMappingData::planeVAO);
        glDeleteVertexArrays(1, &ShadowMappingData::cubeVAO);

        delete ShadowMappingData::shader;
        delete ShadowMappingData::simpleDepthShader;
        delete ShadowMappingData::debugDepthQuad;
    }

    static void renderScene(ShaderProgram *shader, float currentFrame)
    {

        glFrontFace(GL_CW);
        // floor
        glm::mat4 model = glm::mat4(1.0f);
        shader->set4Matrix("model", model);
        glBindVertexArray(ShadowMappingData::planeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glFrontFace(GL_CCW);

        // cubes
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 1.5f, 0.0f));
        model = glm::scale(model, glm::vec3(0.5f));
        shader->set4Matrix("model", model);
        renderCube();
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(2.0f, 0.0f, 1.0f));
        model = glm::scale(model, glm::vec3(0.5f));
        shader->set4Matrix("model", model);
        renderCube();
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-1.0f, 1.5f, 2.0f ));
        model = glm::rotate(model, glm::radians(360.0f * std::sin(currentFrame * 0.02f)), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));        
        model = glm::scale(model, glm::vec3(0.25f));
        shader->set4Matrix("model", model);
        renderCube();                

    }

    static void renderCube()
    {
        if(ShadowMappingData::cubeVAO == 0)
        {
            unsigned int cubeVBO;
            glGenVertexArrays(1, &ShadowMappingData::cubeVAO);
            glGenBuffers(1, &cubeVBO);
            glBindVertexArray(ShadowMappingData::cubeVAO);
            glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(ShadowMappingData::cubeVertices), ShadowMappingData::cubeVertices, GL_STATIC_DRAW);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
            glEnableVertexAttribArray(2);
            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindVertexArray(0);
        }

        glBindVertexArray(ShadowMappingData::cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
    }

    static void renderQuad()
    {
        if(ShadowMappingData::quadVAO == 0)
        {
            unsigned int quadVBO;
            glGenVertexArrays(1, &ShadowMappingData::quadVAO);
            glGenBuffers(1, &quadVBO);
            glBindVertexArray(ShadowMappingData::quadVAO);
            glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(ShadowMappingData::quadVertices), &ShadowMappingData::quadVertices, GL_STATIC_DRAW);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        }

        glBindVertexArray(ShadowMappingData::quadVAO);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
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