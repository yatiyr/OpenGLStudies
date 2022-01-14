#ifndef __INSTANCING_H__
#define __INSTANCING_H__

#include <Lessons/LessonEssentials.h>
#include <Lessons/Instancing/InstancingData.h>
#include <Model.h>


class Instancing
{
public:

    static void initialConf()
    {
        // configure global opengl state
        // -----------------------------
        glEnable(GL_DEPTH_TEST);

        InstancingData::asteroidShader = new ShaderProgram("shaders/instancing/asteroids.vert",
                                                           "shaders/instancing/asteroids.frag");

        InstancingData::planetShader = new ShaderProgram("shaders/instancing/planet.vert",
                                                         "shaders/instancing/planet.frag");

        std::string planetPath = ROOT_DIR + std::string("assets/models/planet/planet.obj");
        InstancingData::planet = new Model(planetPath.c_str());        

        std::string rockPath = ROOT_DIR + std::string("assets/models/rock/rock.obj");
        InstancingData::rock = new Model(rockPath.c_str());       

        InstancingData::modelMatrices = new glm::mat4[InstancingData::amount];
        srand(static_cast<unsigned int>(glfwGetTime())); // initialize random seed
        float radius = 450.0;
        float offset = 75.0f;
        for (unsigned int i=0; i<InstancingData::amount; i++)
        {
            glm::mat4 model = glm::mat4(1.0f);

            // 1. translation: displace along circle with 'radius' in range [-offset, offset]
            float angle = (float)i/(float)InstancingData::amount * 360.0f;
            float displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
            float x = sin(angle) * radius + displacement;
            displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
            float y = displacement * 0.4f; // keep the height of asteroid field smaller compared to width of x and z
            displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
            float z = cos(angle) * radius + displacement;
            model = glm::translate(model, glm::vec3(x,y,z));

            // 2. scale: Scale between 0.05 and 0.25f
            float scale = static_cast<float>((rand() % 20) / 100.0 + 0.05);
            model = glm::scale(model, glm::vec3(scale));

            // 3. rotation: add random rotation around a (semi)randomly picked rotation axis vector
            float rotAngle = static_cast<float>((rand() % 360));
            model = glm::rotate(model, rotAngle, glm::vec3(0.4f, 0.6f, 0.8f));

            // 4. now add to list of matrices
            InstancingData::modelMatrices[i] = model;
        }


        // configure instanced array
        // -------------------------
        unsigned int buffer;
        glGenBuffers(1, &buffer);
        glBindBuffer(GL_ARRAY_BUFFER, buffer);
        glBufferData(GL_ARRAY_BUFFER, InstancingData::amount * sizeof(glm::mat4), &InstancingData::modelMatrices[0], GL_STATIC_DRAW);

        // set transformation matrices as an instance vertex attribute (with divisor 1)
        for (unsigned int i=0; i<InstancingData::rock->meshes.size(); i++)
        {
            unsigned int VAO = InstancingData::rock->meshes[i].VAO;
            glBindVertexArray(VAO);
            // set attribute pointers for matrix (4 times vec4)
            glEnableVertexAttribArray(3);
            glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
            glEnableVertexAttribArray(4);
            glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
            glEnableVertexAttribArray(5);
            glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
            glEnableVertexAttribArray(6);
            glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));

            glVertexAttribDivisor(3,1);
            glVertexAttribDivisor(4,1);
            glVertexAttribDivisor(5,1);
            glVertexAttribDivisor(6,1);            

            glBindVertexArray(0);
        }
    }

    static void draw(Camera* cam)
    {
        // render
        // ------
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        // configure transformation matrices
        glm::mat4 projection = glm::perspective(glm::radians(cam->Zoom), (float)InstancingData::SCR_WIDTH / (float)InstancingData::SCR_HEIGHT, 0.1f, 1000.0f);
        glm::mat4 view = cam->GetViewMatrix();
        InstancingData::asteroidShader->use();
        InstancingData::asteroidShader->set4Matrix("projection", projection);
        InstancingData::asteroidShader->set4Matrix("view", view);
        InstancingData::planetShader->use();
        InstancingData::planetShader->set4Matrix("projection", projection);
        InstancingData::planetShader->set4Matrix("view", view);

        // draw planet
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, -3.0f, 0.0f));
        model = glm::scale(model, glm::vec3(20.0f, 20.0f, 20.0f));
        InstancingData::planetShader->set4Matrix("model", model);
        InstancingData::planet->Draw(*InstancingData::planetShader);

        // draw meteorites
        InstancingData::asteroidShader->use();
        InstancingData::asteroidShader->setInt("texture_diffuse1", 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, InstancingData::rock->textures_loaded[0].id);      
        for (unsigned int i=0; i<InstancingData::rock->meshes.size(); i++)
        {        
            glBindVertexArray(InstancingData::rock->meshes[i].VAO);
            glDrawElementsInstanced(GL_TRIANGLES, static_cast<unsigned int>(InstancingData::rock->meshes[i].indices.size()), GL_UNSIGNED_INT, 0, InstancingData::amount);
            glBindVertexArray(0);
        }
    }


    static void clean()
    {
        delete InstancingData::planet;
        delete InstancingData::rock;

        delete InstancingData::planetShader;
        delete InstancingData::asteroidShader;
    }

};


#endif