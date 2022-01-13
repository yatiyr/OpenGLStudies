#ifndef __GEOMETRY_SHADER_H__
#define __GEOMETRY_SHADER_H__

#include <Lessons/LessonEssentials.h>
#include <Lessons/GeometryShader/GeometryShaderData.h>

class GeometryShader
{
public:

    static void initialConf()
    {
        // flip loaded texture vertically
        setVerticalFlip(true);        
        // configure global opengl state
        glEnable(GL_DEPTH_TEST);

        // discard back facing triangles
        glCullFace(GL_BACK);        

        GeometryShaderData::shader = new ShaderProgram("shaders/geometry/shader.vert",
                                                       "shaders/geometry/shader.frag");

        GeometryShaderData::normalShader = new ShaderProgram("shaders/geometry/normalShader.vert",
                                                             "shaders/geometry/normalShader.frag",
                                                             "shaders/geometry/normalShader.geom");

        std::string backpackPath = ROOT_DIR + std::string("assets/models/backpack/backpack.obj");
        GeometryShaderData::backpackModel = new Model(backpackPath.c_str());

    }

    static void draw(Camera *cam, const float &currentFrame)
    {
        // render
        // ------
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // configure transformation matrices
        glm::mat4 projection = glm::perspective(glm::radians(cam->Zoom), (float)GeometryShaderData::SCR_WIDTH / (float)GeometryShaderData::SCR_HEIGHT, 1.0f,100.0f);
        glm::mat4 view = cam->GetViewMatrix();
        glm::mat4 model = glm::mat4(1.0f);

        GeometryShaderData::shader->use();
        GeometryShaderData::shader->set4Matrix("projection", projection);
        GeometryShaderData::shader->set4Matrix("view", view);
        GeometryShaderData::shader->set4Matrix("model", model);

        // draw model as usual
        GeometryShaderData::backpackModel->Draw(*GeometryShaderData::shader);

        // then draw model with normal visualizing geometry shader
        GeometryShaderData::normalShader->use();
        GeometryShaderData::normalShader->set4Matrix("projection", projection);
        GeometryShaderData::normalShader->set4Matrix("view", view);
        GeometryShaderData::normalShader->set4Matrix("model", model);
        GeometryShaderData::normalShader->setFloat("time", currentFrame);

        GeometryShaderData::backpackModel->Draw(*GeometryShaderData::normalShader);             
    }


    static void clean()
    {
        delete GeometryShaderData::backpackModel;
        delete GeometryShaderData::shader;
        delete GeometryShaderData::normalShader;        
    }
};



#endif