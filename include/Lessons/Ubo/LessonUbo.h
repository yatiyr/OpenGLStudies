#ifndef __LESSON_UBO_H__
#define __LESSON_UBO_H__

#include <Lessons/LessonEssentials.h>
#include <Lessons/Ubo/LessonUboData.h>

class LessonUbo
{
public:

    static void initialConf()
    {
        // configure global opengl state
        glEnable(GL_DEPTH_TEST);

        // Load Shaders
        LessonUboData::shaderRed    = new ShaderProgram("shaders/ubo/ubo.vert",
                                                        "shaders/ubo/uboRed.frag");

        LessonUboData::shaderGreen  = new ShaderProgram("shaders/ubo/ubo.vert",
                                                        "shaders/ubo/uboGreen.frag");

        LessonUboData::shaderBlue   = new ShaderProgram("shaders/ubo/ubo.vert",
                                                        "shaders/ubo/uboBlue.frag");

        LessonUboData::shaderYellow = new ShaderProgram("shaders/ubo/ubo.vert",
                                                        "shaders/ubo/uboYellow.frag"); 


        glGenVertexArrays(1, &LessonUboData::cubeVAO)                                                        ;
        glGenBuffers(1, &LessonUboData::cubeVBO);
        glBindVertexArray(LessonUboData::cubeVAO);
        glBindBuffer(GL_ARRAY_BUFFER, LessonUboData::cubeVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(LessonUboData::verticesCube), &LessonUboData::verticesCube, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

        // configure a uniform buffer object
        // ---------------------------------
        // first. We get the relevant block indices
        unsigned int uniformBlockIndexRed = glGetUniformBlockIndex(LessonUboData::shaderRed->id, "Matrices");
        unsigned int uniformBlockIndexGreen = glGetUniformBlockIndex(LessonUboData::shaderGreen->id, "Matrices");
        unsigned int uniformBlockIndexBlue = glGetUniformBlockIndex(LessonUboData::shaderBlue->id, "Matrices");
        unsigned int uniformBlockIndexYellow = glGetUniformBlockIndex(LessonUboData::shaderYellow->id, "Matrices");
        // then we link each shader's uniform block to this uniform binding point
        glUniformBlockBinding(LessonUboData::shaderRed->id, uniformBlockIndexRed, 0);
        glUniformBlockBinding(LessonUboData::shaderGreen->id, uniformBlockIndexGreen, 0);
        glUniformBlockBinding(LessonUboData::shaderBlue->id, uniformBlockIndexBlue, 0);
        glUniformBlockBinding(LessonUboData::shaderYellow->id, uniformBlockIndexYellow, 0);
        // create uniform buffer object
        glGenBuffers(1, &LessonUboData::uboMatrices);
        glBindBuffer(GL_UNIFORM_BUFFER, LessonUboData::uboMatrices);
        glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), NULL, GL_STATIC_DRAW);
        // define the range of the buffer that links to a uniform binding point
        glBindBufferRange(GL_UNIFORM_BUFFER, 0, LessonUboData::uboMatrices, 0, 2 * sizeof(glm::mat4));

        // store the projection matrix
        glm::mat4 projection = glm::perspective(45.0f, (float)LessonUboData::SCR_WIDTH / (float)LessonUboData::SCR_HEIGHT, 0.1f, 100.0f);
        glBindBuffer(GL_UNIFORM_BUFFER, LessonUboData::uboMatrices);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(projection));
        glBindBuffer(GL_UNIFORM_BUFFER, 0);


    }

    static void draw(Camera *cam)
    {
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // set view matrix
        glm::mat4 view = cam->GetViewMatrix();
        glBindBuffer(GL_UNIFORM_BUFFER, LessonUboData::uboMatrices);
        glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(view));
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        // draw 4 cubes 
        // RED
        glBindVertexArray(LessonUboData::cubeVAO);
        LessonUboData::shaderRed->use();
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-0.75f, 0.75f, 0.0f)); // move top-left
        LessonUboData::shaderRed->set4Matrix("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        // GREEN
        LessonUboData::shaderGreen->use();
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.75f, 0.75f, 0.0f)); // move top-right
        LessonUboData::shaderGreen->set4Matrix("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        // YELLOW
        LessonUboData::shaderYellow->use();
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-0.75f, -0.75f, 0.0f)); // move bottom-left
        LessonUboData::shaderYellow->set4Matrix("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        // BLUE
        LessonUboData::shaderBlue->use();
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.75f, -0.75f, 0.0f)); // move bottom-right
        LessonUboData::shaderBlue->set4Matrix("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);        
    }

    static void clean()
    {                                                                                                         
        glDeleteVertexArrays(1, &LessonUboData::cubeVAO);
        glDeleteBuffers(1, &LessonUboData::cubeVBO);
        glDeleteBuffers(1, &LessonUboData::uboMatrices);
    }
};


#endif