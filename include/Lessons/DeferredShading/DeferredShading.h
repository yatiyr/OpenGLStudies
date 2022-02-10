#ifndef __DEFERRED_SHADING_H__
#define __DEFERRED_SHADING_H__

#include <Lessons/LessonEssentials.h>
#include <Lessons/DeferredShading/DeferredShadingData.h>
#include <Model.h>
#include <stdlib.h>

#include <random>

class DeferredShading
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
        DeferredShadingData::shaderGeometryPass = new ShaderProgram("shaders/deferredShading/g_buffer.vert",
                                                                    "shaders/deferredShading/g_buffer.frag");

        DeferredShadingData::shaderLightingPass = new ShaderProgram("shaders/deferredShading/deferred_shading.vert",
                                                                    "shaders/deferredShading/deferred_shading.frag");

        DeferredShadingData::shaderLightBox = new ShaderProgram("shaders/deferredShading/deferred_light_box.vert",
                                                                "shaders/deferredShading/deferred_light_box.frag");

        std::string backpackPath = ROOT_DIR + std::string("assets/models/backpack/backpack.obj");
        DeferredShadingData::backpack = new Model(backpackPath.c_str());

        DeferredShadingData::objectPositions.push_back(glm::vec3(-3.0, -0.5, -3.0));
        DeferredShadingData::objectPositions.push_back(glm::vec3( 0.0, -0.5, -3.0));
        DeferredShadingData::objectPositions.push_back(glm::vec3( 3.0, -0.5, -3.0));
        DeferredShadingData::objectPositions.push_back(glm::vec3(-3.0, -0.5,  0.0));
        DeferredShadingData::objectPositions.push_back(glm::vec3( 0.0, -0.5,  0.0));
        DeferredShadingData::objectPositions.push_back(glm::vec3( 3.0, -0.5,  0.0));
        DeferredShadingData::objectPositions.push_back(glm::vec3(-3.0, -0.5,  3.0));
        DeferredShadingData::objectPositions.push_back(glm::vec3( 0.0, -0.5,  3.0));
        DeferredShadingData::objectPositions.push_back(glm::vec3( 3.0, -0.5,  3.0));

        glGenFramebuffers(1, &DeferredShadingData::gBuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, DeferredShadingData::gBuffer);

        // position color buffer
        glGenTextures(1, &DeferredShadingData::gPosition);
        glBindTexture(GL_TEXTURE_2D, DeferredShadingData::gPosition);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, DeferredShadingData::SCR_WIDTH, DeferredShadingData::SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, DeferredShadingData::gPosition, 0);

        // normal color buffer
        glGenTextures(1, &DeferredShadingData::gNormal);
        glBindTexture(GL_TEXTURE_2D, DeferredShadingData::gNormal);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, DeferredShadingData::SCR_WIDTH, DeferredShadingData::SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, DeferredShadingData::gNormal, 0);

        // color + specular color buffer
        glGenTextures(1, &DeferredShadingData::gAlbedoSpec);
        glBindTexture(GL_TEXTURE_2D, DeferredShadingData::gAlbedoSpec);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, DeferredShadingData::SCR_WIDTH, DeferredShadingData::SCR_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, DeferredShadingData::gAlbedoSpec, 0);
        
        // tell opengl which color attachments we'll use of this framebuffer for rendering
        unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
        glDrawBuffers(3, attachments);

        // create and attach depth buffer (renderbuffer)
        unsigned int rboDepth;
        glGenRenderbuffers(1, &rboDepth);
        glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, DeferredShadingData::SCR_WIDTH, DeferredShadingData::SCR_HEIGHT);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
        // finally check if framebuffer is complete
        if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "Framebuffer not complete!" << std::endl;
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        std::mt19937 rng(std::random_device{}());
        std::uniform_int_distribution<> dist(0, 100);

        for(unsigned int i=0; i<DeferredShadingData::NR_LIGHTS; i++)
        {
            // calculate slightly random offsets
            float xPos = static_cast<float>(((dist(rng)) / 100.0) * 6.0 - 3.0);
            float yPos = static_cast<float>(((dist(rng)) / 100.0) * 6.0 - 4.0);
            float zPos = static_cast<float>(((dist(rng)) / 100.0) * 6.0 - 3.0);
            DeferredShadingData::lightPositions.push_back(glm::vec3(xPos, yPos, zPos));

            // also calculate random color
            float rColor = static_cast<float>(((dist(rng)) / 200.0) + 0.5);
            float gColor = static_cast<float>(((dist(rng)) / 200.0) + 0.5);
            float bColor = static_cast<float>(((dist(rng)) / 200.0) + 0.5);            
            DeferredShadingData::lightColors.push_back(glm::vec3(rColor, gColor, bColor));
        }


        DeferredShadingData::shaderLightingPass->use();
        DeferredShadingData::shaderLightingPass->setInt("gPosition", 0);
        DeferredShadingData::shaderLightingPass->setInt("gNormal", 1);
        DeferredShadingData::shaderLightingPass->setInt("gAlbedoSpec", 2);

    }

    static void draw(Camera *cam, float currentFrame)
    {
        // render
        // ------
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // geometry pass
        glBindFramebuffer(GL_FRAMEBUFFER, DeferredShadingData::gBuffer);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glm::mat4 projection = glm::perspective(glm::radians(cam->Zoom), (float)DeferredShadingData::SCR_WIDTH / (float)DeferredShadingData::SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = cam->GetViewMatrix();
        glm::mat4 model = glm::mat4(1.0f);
        DeferredShadingData::shaderGeometryPass->use();
        DeferredShadingData::shaderGeometryPass->set4Matrix("projection", projection);
        DeferredShadingData::shaderGeometryPass->set4Matrix("view", view);
        for(unsigned int i=0; i<DeferredShadingData::objectPositions.size(); i++)
        {
            model = glm::mat4(1.0f);
            model = glm::translate(model, DeferredShadingData::objectPositions[i]);
            model = glm::scale(model, glm::vec3(0.25f));
            DeferredShadingData::shaderGeometryPass->set4Matrix("model", model);
            DeferredShadingData::backpack->Draw(*DeferredShadingData::shaderGeometryPass);
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // 2. lighting pass: calculate lighting by iterating over a screen filled quad pixel-by-pixel using the gbuffer's content
        // ----------------------------------------------------------------------------------------------------------------------
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        DeferredShadingData::shaderLightingPass->use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, DeferredShadingData::gPosition);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, DeferredShadingData::gNormal);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, DeferredShadingData::gAlbedoSpec);
        // send light relevant uniforms
        for(unsigned int  i=0; i<DeferredShadingData::lightPositions.size(); i++)
        {
            DeferredShadingData::shaderLightingPass->setVec3("lights[" + std::to_string(i) + "].Position", DeferredShadingData::lightPositions[i]);
            DeferredShadingData::shaderLightingPass->setVec3("lights[" + std::to_string(i) + "].Color", DeferredShadingData::lightColors[i]);
            // update attenuation and calculate radius
            const float constant = 1.0f;
            const float linear = 0.7f;
            const float quadratic = 1.8f;
            DeferredShadingData::shaderLightingPass->setFloat("lights[" + std::to_string(i) + "].Linear", linear);
            DeferredShadingData::shaderLightingPass->setFloat("lights[" + std::to_string(i) + "].Quadratic", quadratic);
            // then calculate radius of light volume/sphere
            const float maxBrightness = std::fmaxf(std::fmaxf(DeferredShadingData::lightColors[i].r, DeferredShadingData::lightColors[i].g), 
                                                   DeferredShadingData::lightColors[i].b);
            float radius = (-linear + std::sqrt(linear * linear - 4 *quadratic * (constant - (256.0f / 5.0f) * maxBrightness))) / (2.0f * quadratic);
            DeferredShadingData::shaderLightingPass->setFloat("lights[" + std::to_string(i) + "].Radius", radius);
        }
        DeferredShadingData::shaderLightingPass->setVec3("viewPos", cam->Position);
        renderQuad();

        // copy content of geometry's depth buffer to default framebuffer's depth buffer
        // -----------------------------------------------------------------------------
        glBindFramebuffer(GL_READ_FRAMEBUFFER, DeferredShadingData::gBuffer);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

        glBlitFramebuffer(0, 0, DeferredShadingData::SCR_WIDTH, DeferredShadingData::SCR_HEIGHT, 0, 0, DeferredShadingData::SCR_WIDTH, DeferredShadingData::SCR_HEIGHT, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // render lights on top of scene
        // -----------------------------
        DeferredShadingData::shaderLightBox->use();
        DeferredShadingData::shaderLightBox->set4Matrix("projection", projection);
        DeferredShadingData::shaderLightBox->set4Matrix("view", view);
        for(unsigned int i=0; i<DeferredShadingData::lightPositions.size(); i++)
        {
            model = glm::mat4(1.0f);
            model = glm::translate(model, DeferredShadingData::lightPositions[i]);
            model = glm::scale(model, glm::vec3(0.125f));
            DeferredShadingData::shaderLightBox->set4Matrix("model", model);
            DeferredShadingData::shaderLightBox->setVec3("lightColor", DeferredShadingData::lightColors[i]);
            renderCube();
        }
    }

    static void clean()
    {
        glDeleteVertexArrays(1, &DeferredShadingData::cubeVAO);
        glDeleteVertexArrays(1, &DeferredShadingData::quadVAO);

        delete DeferredShadingData::shaderGeometryPass;
        delete DeferredShadingData::shaderLightingPass;
        delete DeferredShadingData::shaderLightBox;
    }

    static void renderCube()
    {
        if(DeferredShadingData::cubeVAO == 0)
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
            glGenVertexArrays(1, &DeferredShadingData::cubeVAO);
            glGenBuffers(1, &cubeVBO);
            // fill buffer
            glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);
            glBindVertexArray(DeferredShadingData::cubeVAO);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
            glEnableVertexAttribArray(2);
            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindVertexArray(0);
        }

        glBindVertexArray(DeferredShadingData::cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);        
    }

    static void renderQuad()
    {
        if(DeferredShadingData::quadVAO == 0)
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
            glGenVertexArrays(1, &DeferredShadingData::quadVAO);
            glGenBuffers(1, &quadVBO);
            glBindVertexArray(DeferredShadingData::quadVAO);
            glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));            
        }

        glBindVertexArray(DeferredShadingData::quadVAO);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindVertexArray(0);        
    }    
};

#endif