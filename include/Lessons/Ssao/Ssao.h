#ifndef __SSAO_H__
#define __SSAO_H__

#include <Lessons/LessonEssentials.h>
#include <Lessons/Ssao/SsaoData.h>
#include <Model.h>
#include <random>

float lerp(float a, float b, float f)
{
    return a + f * (b-a);
}

class Ssao
{
public:
    static void initialConf()
    {
        stbi_set_flip_vertically_on_load(true);

        // configure global opengl state
        // -----------------------------
        glEnable(GL_DEPTH_TEST);

        // build and compile shaders
        // -------------------------
        SsaoData::shaderGeometryPass = new ShaderProgram("shaders/ssao/ssao_geometry.vert",
                                                         "shaders/ssao/ssao_geometry.frag");

        SsaoData::shaderLightingPass = new ShaderProgram("shaders/ssao/ssao.vert",
                                                         "shaders/ssao/ssao_lighting.frag");

        SsaoData::shaderSSAO = new ShaderProgram("shaders/ssao/ssao.vert",
                                                 "shaders/ssao/ssao.frag");

        SsaoData::shaderSSAOBlur = new ShaderProgram("shaders/ssao/ssao.vert",
                                                     "shaders/ssao/ssao_blur.frag");

        // load models
        // -----------
        std::string backpackPath = ROOT_DIR + std::string("assets/models/backpack/backpack.obj");
        SsaoData::backpack = new Model(backpackPath.c_str());

        SsaoData::sphere = new Sphere();

        glGenFramebuffers(1, &SsaoData::gBuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, SsaoData::gBuffer);

        // position color buffer
        glGenTextures(1, &SsaoData::gPosition);
        glBindTexture(GL_TEXTURE_2D, SsaoData::gPosition);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SsaoData::SCR_WIDTH, SsaoData::SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, SsaoData::gPosition, 0);

        // normal color buffer
        glGenTextures(1, &SsaoData::gNormal);
        glBindTexture(GL_TEXTURE_2D, SsaoData::gNormal);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SsaoData::SCR_WIDTH, SsaoData::SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, SsaoData::gNormal, 0);

        // color + specular color buffer
        glGenTextures(1, &SsaoData::gAlbedo);
        glBindTexture(GL_TEXTURE_2D, SsaoData::gAlbedo);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, SsaoData::SCR_WIDTH, SsaoData::SCR_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, SsaoData::gAlbedo, 0);

        unsigned int attachments[3] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2};
        glDrawBuffers(3, attachments);

        // create depth buffer and attach it to currently binded framebuffer
        unsigned int rboDepth;
        glGenRenderbuffers(1, &rboDepth);
        glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SsaoData::SCR_WIDTH, SsaoData::SCR_HEIGHT);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
        // check framebuffer
        if(glCheckFramebufferStatus(GL_FRAMEBUFFER) !=  GL_FRAMEBUFFER_COMPLETE)
            std::cout << "Framebuffer not complete" << std::endl;
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // also create framebuffer to hold SSAO processing stage
        // -----------------------------------------------------
        glGenFramebuffers(1, &SsaoData::ssaoFBO);
        glGenFramebuffers(1, &SsaoData::ssaoBlurFBO);

        glBindFramebuffer(GL_FRAMEBUFFER, SsaoData::ssaoFBO);
        // ssao color buffer
        glGenTextures(1, &SsaoData::ssaoColorBuffer);
        glBindTexture(GL_TEXTURE_2D, SsaoData::ssaoColorBuffer);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, SsaoData::SCR_WIDTH, SsaoData::SCR_HEIGHT, 0, GL_RED, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, SsaoData::ssaoColorBuffer, 0);
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "SSAO Framebuffer not complete!" << std::endl;

        // and blur stage
        glBindFramebuffer(GL_FRAMEBUFFER, SsaoData::ssaoBlurFBO);
        glGenTextures(1, &SsaoData::ssaoColorBufferBlur);
        glBindTexture(GL_TEXTURE_2D, SsaoData::ssaoColorBufferBlur);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, SsaoData::SCR_WIDTH, SsaoData::SCR_HEIGHT, 0, GL_RED, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, SsaoData::ssaoColorBufferBlur, 0);
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "SSAO Framebuffer not complete!" << std::endl;

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // generate sample kernel
        // ----------------------
        std::uniform_real_distribution<GLfloat> randomFloats(0.0, 1.0);
        std::default_random_engine generator;
        std::vector<glm::vec3> ssaoKernel;
        for(unsigned int i=0; i<64; i++)
        {
            glm::vec3 sample(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, randomFloats(generator));
            sample = glm::normalize(sample);
            sample *= randomFloats(generator);
            float scale = float(i) / 64.0f;

            // scale samples so that they are more aligned to center of kernel
            scale = lerp(0.1f, 1.0f, scale * scale);
            sample *= scale;
            SsaoData::ssaoKernel.push_back(sample);
        }

        // generate noise texture
        // ----------------------
        for(unsigned int i=0; i<16; i++)
        {
            // for rotating aroung z-axis
            glm::vec3 noise(randomFloats(generator) * 2.0 - 1.0,
                            randomFloats(generator) * 2.0 - 1.0, 0.0f);

            SsaoData::ssaoNoise.push_back(noise);
        }
        
        glGenTextures(1, &SsaoData::noiseTexture);
        glBindTexture(GL_TEXTURE_2D, SsaoData::noiseTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 4, 4, 0, GL_RGB, GL_FLOAT, &SsaoData::ssaoNoise[0]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        // shader configuration
        // --------------------
        SsaoData::shaderLightingPass->use();
        SsaoData::shaderLightingPass->setInt("gPosition", 0);
        SsaoData::shaderLightingPass->setInt("gNormal", 1);
        SsaoData::shaderLightingPass->setInt("gAlbedo", 2);
        SsaoData::shaderLightingPass->setInt("ssao", 3);

        SsaoData::shaderSSAO->use();
        SsaoData::shaderSSAO->setInt("gPosition", 0);
        SsaoData::shaderSSAO->setInt("gNormal", 1);
        SsaoData::shaderSSAO->setInt("texNoise", 2);

        SsaoData::shaderSSAOBlur->use();
        SsaoData::shaderSSAOBlur->setInt("ssaoInput", 0);

    }

    static void draw(Camera *cam, float currentFrame)
    {

        // render
        // ------
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // 1. geometry pass: render scene's geometry/color data into gbuffer
        // -----------------------------------------------------------------
        glBindFramebuffer(GL_FRAMEBUFFER, SsaoData::gBuffer);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glm::mat4 projection = glm::perspective(glm::radians(cam->Zoom), (float)SsaoData::SCR_WIDTH / (float)SsaoData::SCR_HEIGHT, 0.1f, 50.0f);
            glm::mat4 view = cam->GetViewMatrix();
            glm::mat4 model = glm::mat4(1.0f);
            SsaoData::shaderGeometryPass->use();
            SsaoData::shaderGeometryPass->set4Matrix("projection", projection);
            SsaoData::shaderGeometryPass->set4Matrix("view", view);
            // room cube
            model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(0.0f, 7.0f, 0.0f));
            model = glm::scale(model, glm::vec3(7.5f, 7.5f, 7.5f));
            SsaoData::shaderGeometryPass->set4Matrix("model", model);
            SsaoData::shaderGeometryPass->setInt("invertedNormals", 1);
            renderCube();
            SsaoData::shaderGeometryPass->setInt("invertedNormals", 0);
            // backpack model on the floor
            model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(0.0f, 0.5f, 0.0f));
            model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
            model = glm::scale(model, glm::vec3(1.0f));
            SsaoData::shaderGeometryPass->set4Matrix("model", model);
            SsaoData::backpack->Draw(*SsaoData::shaderGeometryPass);

            model = glm::translate(model, glm::vec3(0.0f, 3.0f, 0.0f));
            model = glm::scale(model, glm::vec3(0.6f, 0.6f, 0.6f));
            SsaoData::shaderGeometryPass->set4Matrix("model", model);
            SsaoData::sphere->draw();
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // 2. generate SSAO texture
        // ------------------------
        glBindFramebuffer(GL_FRAMEBUFFER, SsaoData::ssaoFBO);
            glClear(GL_COLOR_BUFFER_BIT);
            SsaoData::shaderSSAO->use();
            // send kernel + rotation
            for(unsigned int i=0; i<64; i++)
                SsaoData::shaderSSAO->setVec3("samples[" + std::to_string(i) + "]", SsaoData::ssaoKernel[i]);
            SsaoData::shaderSSAO->set4Matrix("projection", projection);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, SsaoData::gPosition);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, SsaoData::gNormal);
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, SsaoData::noiseTexture);
            renderQuad();
        glBindFramebuffer(GL_FRAMEBUFFER, 0);


        // 3. blur SSAO texture to remove noise
        // ------------------------------------
        glBindFramebuffer(GL_FRAMEBUFFER, SsaoData::ssaoBlurFBO);
            glClear(GL_COLOR_BUFFER_BIT);
            SsaoData::shaderSSAOBlur->use();
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, SsaoData::ssaoColorBuffer);
            renderQuad();
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // 4. lighting pass: traditional deferred blinn-phong lighting with added
        // screen-space ambient occlusion
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        SsaoData::shaderLightingPass->use();
        // send light relevant uniforms
        glm::vec3 lightPosView = glm::vec3(cam->GetViewMatrix() * glm::vec4(SsaoData::lightPos, 1.0));
        SsaoData::shaderLightingPass->setVec3("light.Position", lightPosView);
        SsaoData::shaderLightingPass->setVec3("light.Color", SsaoData::lightColor);
        // update attenuation parameters
        const float linear = 0.09;
        const float quadratic = 0.32f;
        SsaoData::shaderLightingPass->setFloat("light.Linear", linear);
        SsaoData::shaderLightingPass->setFloat("light.Quadratic", quadratic);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, SsaoData::gPosition);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, SsaoData::gNormal);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, SsaoData::gAlbedo);
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, SsaoData::ssaoColorBufferBlur);
        renderQuad();
    }

    static void clean()
    {
        glDeleteVertexArrays(1, &SsaoData::cubeVAO);
        glDeleteVertexArrays(1, &SsaoData::quadVAO);

        delete SsaoData::shaderGeometryPass;
        delete SsaoData::shaderLightingPass;
        delete SsaoData::shaderSSAO;
        delete SsaoData::shaderSSAOBlur;
    }

    static void renderCube()
    {
        if(SsaoData::cubeVAO == 0)
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
            glGenVertexArrays(1, &SsaoData::cubeVAO);
            glGenBuffers(1, &cubeVBO);
            // fill buffer
            glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);
            glBindVertexArray(SsaoData::cubeVAO);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
            glEnableVertexAttribArray(2);
            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindVertexArray(0);
        }

        glBindVertexArray(SsaoData::cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);   
    }

    static void renderQuad()
    {
        if(SsaoData::quadVAO == 0)
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
            glGenVertexArrays(1, &SsaoData::quadVAO);
            glGenBuffers(1, &quadVBO);
            glBindVertexArray(SsaoData::quadVAO);
            glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));            
        }

        glBindVertexArray(SsaoData::quadVAO);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindVertexArray(0);  
    }
};



#endif