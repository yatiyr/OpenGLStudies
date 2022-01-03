#ifndef __LESSON_STENCIL_H__
#define __LESSON_STENCIL_H__

#include <Lessons/LessonEssentials.h>
#include <Lessons/Stencil/LessonStencilData.h>

class LessonStencil
{
public:


    static void writeLightsToShader(Camera* cam)
    {
        // give light properties to shader
        glm::vec3 lightColor;
        lightColor.x = 1.0f;
        lightColor.y = 1.0f;
        lightColor.z = 1.0f;

        glm::vec3 diffuseColor = lightColor * glm::vec3(0.2f);
        glm::vec3 ambientColor = diffuseColor * glm::vec3(0.1f);    

        // Directional light
        LessonStencilData::modelProgram->setVec3("directionalLight.direction", -0.2f, -1.0f, -0.3f);
        LessonStencilData::modelProgram->setVec3("directionalLight.ambient", ambientColor);
        LessonStencilData::modelProgram->setVec3("directionalLight.diffuse", diffuseColor);
        LessonStencilData::modelProgram->setVec3("directionalLight.specular", 1.0f, 1.0f, 1.0f);

        for(int i=0; i<4; i++)
        {
            glm::vec3 diffuseColor = LessonStencilData::pointLightColors[i] * glm::vec3(0.7);
            glm::vec3 ambientColor = LessonStencilData::pointLightColors[i] * glm::vec3(0.5f);

            LessonStencilData::modelProgram->setVec3("pointLights[" + std::to_string(i) + "].position", LessonStencilData::pointLightPositions[i]);
            LessonStencilData::modelProgram->setVec3("pointLights[" + std::to_string(i) + "].ambient", ambientColor);
            LessonStencilData::modelProgram->setVec3("pointLights[" + std::to_string(i) + "].diffuse", diffuseColor);
            LessonStencilData::modelProgram->setVec3("pointLights[" + std::to_string(i) + "].specular", 1.0f, 1.0f, 1.0f);

            LessonStencilData::modelProgram->setFloat("pointLights[" + std::to_string(i) + "].constant", 1.0f);
            LessonStencilData::modelProgram->setFloat("pointLights[" + std::to_string(i) + "].linear", 0.2f);
            LessonStencilData::modelProgram->setFloat("pointLights[" + std::to_string(i) + "].quadratic", 0.302f);              
        }

        // Spot Light
        LessonStencilData::modelProgram->setVec3("spotLight.position", cam->Position);
        LessonStencilData::modelProgram->setVec3("spotLight.direction", cam->Front);
        LessonStencilData::modelProgram->setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
        LessonStencilData::modelProgram->setFloat("spotLight.outerCutOff", glm::cos(glm::radians(17.5f)));

        LessonStencilData::modelProgram->setVec3("spotLight.ambient", ambientColor);
        LessonStencilData::modelProgram->setVec3("spotLight.diffuse", diffuseColor);
        LessonStencilData::modelProgram->setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);

        LessonStencilData::modelProgram->setFloat("spotLight.constant", 1.0f);
        LessonStencilData::modelProgram->setFloat("spotLight.linear", 0.03f);
        LessonStencilData::modelProgram->setFloat("spotLight.quadratic", 0.012f);        
    }

    static void drawModels(Camera *cam)
    {
        LessonStencilData::modelProgram->use();
        glStencilMask(0x00);

        // Send view position to shader program
        LessonStencilData::modelProgram->setVec3("viewPos", cam->Position);

        // Pass projection matrix to shader -> changes every frame because of camera
        glm::mat4 projection = glm::perspective(glm::radians(cam->Zoom), (float) LessonStencilData::SRC_WIDTH / (float) LessonStencilData::SRC_HEIGHT, 0.1f, 10000.0f);
        glm::mat4 view = cam->GetViewMatrix();
        LessonStencilData::modelProgram->set4Matrix("projection", projection);
        LessonStencilData::modelProgram->set4Matrix("view", view);

        glm::mat4 model(1.0f);
        model = glm::translate(model, glm::vec3(-1.0f, 2.0f, -3.0f));
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));

        glm::mat3 normalMatrix = glm::transpose(glm::inverse(model));

        glm::mat4 model2(1.0f);
        model2 = glm::translate(model2, glm::vec3(0.0f, -2.0f, 0.0f));
        model2 = glm::scale(model2, glm::vec3(0.1f, 0.1f, 0.1f));
        LessonStencilData::modelProgram->set4Matrix("model", model2);
        LessonStencilData::sponzaModel->Draw(*LessonStencilData::modelProgram);

        LessonStencilData::modelProgram->set4Matrix("model", model);
        LessonStencilData::modelProgram->set3Matrix("normalMatrix", normalMatrix);
        LessonStencilData::backpackModel->Draw(*LessonStencilData::modelProgram);

        glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
        glDisable(GL_DEPTH_TEST);
        glStencilFunc(GL_ALWAYS, 1, 0xFF);
        glStencilMask(0xFF);
        LessonStencilData::backpackModel->Draw(*LessonStencilData::modelProgram);

        glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
        glStencilMask(0x00);
        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
        LessonStencilData::singleColorProgram->use();

        glm::mat4 outlineModel = glm::scale(model, glm::vec3(1.0f,1.0f,1.0f));
        LessonStencilData::singleColorProgram->set4Matrix("model", outlineModel);
        LessonStencilData::singleColorProgram->set4Matrix("view", view);
        LessonStencilData::singleColorProgram->set4Matrix("projection", projection);
        LessonStencilData::backpackModel->Draw(*LessonStencilData::singleColorProgram);
        glStencilMask(0xFF);
        glEnable(GL_DEPTH_TEST);
        glStencilFunc(GL_ALWAYS, 0, 0xFF);
          
    }

    static void initialConf()
    {

        // flip loaded textures vertically
        setVerticalFlip(true);        

        // Global OpenGL state -> depth test
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);

        // Enable Stencil testing
        glEnable(GL_STENCIL_TEST);
        glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
        glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
        glStencilMask(0x00);

        // Load Shaders
        LessonStencilData::modelProgram = new ShaderProgram("shaders/modelShader/model_shader.vert",
                                    "shaders/modelShader/model_shader.frag");

        LessonStencilData::lightProgram = new ShaderProgram("shaders/lightShader/shader_light.vert",
                                     "shaders/lightShader/shader_light.frag");

        LessonStencilData::singleColorProgram = new ShaderProgram("shaders/shaderSingleColor/shaderSingleColor.vert",
                                           "shaders/shaderSingleColor/shaderSingleColor.frag");

        // Load Models
        std::string backpackPath = ROOT_DIR + std::string("assets/models/backpack/backpack.obj");
        LessonStencilData::backpackModel = new Model(backpackPath.c_str());

        std::string sponzaPath = ROOT_DIR + std::string("assets/models/sponza/Sponza.gltf");
        LessonStencilData::sponzaModel = new Model(sponzaPath.c_str());

        // ----------------------- FOR CREATING LIGHT BUFFERS ------------------ //
        glGenVertexArrays(1, &LessonStencilData::lightVAO);
        glBindVertexArray(LessonStencilData::lightVAO);
        glGenBuffers(1, &LessonStencilData::lightVBO);

        // We bind VBO to GL_ARRAY_BUFFER because it sends vertex data
        // from now on, every buffer calls we make on GL_ARRAY_BUFFER
        // will be used to configure VBO
        glBindBuffer(GL_ARRAY_BUFFER, LessonStencilData::lightVBO);        

        // We call glBufferData to copy previously defined vertex data
        // into buffer's memory
        // GL_STATIC_DRAW : the data is set only once and used many times
        // GL_STREAM_DRAW : the data is set only once and used by the GPU
        //                  at most a few times
        // GL_DYNAMIC_DRAW : the date is changed a lot and used many times
        glBufferData(GL_ARRAY_BUFFER, sizeof(LessonStencilData::verticesDiffuseMap), LessonStencilData::verticesDiffuseMap, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glBindVertexArray(0);

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);


        LessonStencilData::modelProgram->use();
        LessonStencilData::modelProgram->setFloat("material.shininess", 32.0f);
    

    }

    static void draw(Camera *cam, const float &currentFrame)
    {
        // Specify clear color
        // This is a state setting function
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

        // This is a state-using function
        // it clears the buffer with clear
        // color we specify
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        LessonStencilData::modelProgram->use();
        LessonStencilData::modelProgram->setFloat("time", currentFrame);

        glm::mat4 projection = glm::perspective(glm::radians(cam->Zoom), (float) LessonStencilData::SRC_WIDTH / (float) LessonStencilData::SRC_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = cam->GetViewMatrix();
        LessonStencilData::modelProgram->set4Matrix("projection", projection);
        LessonStencilData::modelProgram->set4Matrix("view", view);

        glBindVertexArray(LessonStencilData::lightVAO);
        LessonStencilData::lightProgram->use();
        LessonStencilData::lightProgram->set4Matrix("projection", projection);
        LessonStencilData::lightProgram->set4Matrix("view", view);

        for(int i=0; i<4; i++)
        {
            glm::mat4 modelLight = glm::mat4(1.0f);
            modelLight = glm::translate(modelLight, LessonStencilData::pointLightPositions[i]);
            modelLight = glm::scale(modelLight, glm::vec3(0.1f));

            LessonStencilData::lightProgram->set4Matrix("model", modelLight);
            LessonStencilData::lightProgram->setVec3("uColor", LessonStencilData::pointLightColors[i]);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }


        LessonStencilData::modelProgram->use();

        writeLightsToShader(cam);
        drawModels(cam);

    }

    static void clean()
    {
        glDeleteVertexArrays(1, &LessonStencilData::lightVAO);
        glDeleteBuffers(1, &LessonStencilData::lightVBO);
        glDeleteProgram(LessonStencilData::modelProgram->id);
        glDeleteProgram(LessonStencilData::lightProgram->id);
        glDeleteProgram(LessonStencilData::singleColorProgram->id);

        delete LessonStencilData::modelProgram;
        delete LessonStencilData::lightProgram;
        delete LessonStencilData::singleColorProgram;
        delete LessonStencilData::backpackModel;
        delete LessonStencilData::sponzaModel;
    }

};



#endif