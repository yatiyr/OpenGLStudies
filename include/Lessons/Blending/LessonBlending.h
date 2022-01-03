#ifndef __LESSONS_H__
#define __LESSONS_H__

#include <Lessons/LessonEssentials.h>
#include <Lessons/Blending/LessonBlendingData.h>

#include <map>

class LessonBlending {
public:

    static void initialConf()
    {

        // flip loaded textures vertically
        setVerticalFlip(false);

        // Global opengl state specific to this lesson
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        LessonBlendingData::blendingShader = new ShaderProgram("shaders/blending/blending.vert","shaders/blending/blending.frag");

        // prepare LessonBlendingData::cubeVAO
        glGenVertexArrays(1, &LessonBlendingData::cubeVAO);
        glGenBuffers(1, &LessonBlendingData::cubeVBO);
        glBindVertexArray(LessonBlendingData::cubeVAO);
        glBindBuffer(GL_ARRAY_BUFFER, LessonBlendingData::cubeVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(LessonBlendingData::verticesCube), &LessonBlendingData::verticesCube, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));


        // prepare LessonBlendingData::planeVAO
        glGenVertexArrays(1, &LessonBlendingData::planeVAO);
        glGenBuffers(1, &LessonBlendingData::planeVBO);
        glBindVertexArray(LessonBlendingData::planeVAO);
        glBindBuffer(GL_ARRAY_BUFFER, LessonBlendingData::planeVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(LessonBlendingData::planeVertices), &LessonBlendingData::planeVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

        // prepare transparent VAO
        glGenVertexArrays(1, &LessonBlendingData::transparentVAO);
        glGenBuffers(1, &LessonBlendingData::transparentVBO);
        glBindVertexArray(LessonBlendingData::transparentVAO);
        glBindBuffer(GL_ARRAY_BUFFER, LessonBlendingData::transparentVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(LessonBlendingData::transparentVertices), &LessonBlendingData::transparentVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

        glBindVertexArray(0);


        // load textures
        std::string cubeTexturePath = std::string(ROOT_DIR) + "assets/textures/blend/marble.png";
        std::string floorTexturePath = std::string(ROOT_DIR) + "assets/textures/blend/metal.png";
        std::string transparentTexture = std::string(ROOT_DIR) + "assets/textures/blend/window.png";

        LessonBlendingData::cubeTexture = LoadTexture(cubeTexturePath.c_str());
        LessonBlendingData::floorTexture = LoadTexture(floorTexturePath.c_str());
        LessonBlendingData::transparentTexture = LoadTexture(transparentTexture.c_str());

        // use shader
        LessonBlendingData::blendingShader->use();
        LessonBlendingData::blendingShader->setInt("texture1", 0);


    }

    static void draw(Camera *cam)
    {
        // sort the transparent windows before rendering
        // ---------------------------------------------
        std::map<float, glm::vec3> sorted;
        for (unsigned int i=0; i<LessonBlendingData::windows.size(); i++)
        {
            float distance = glm::length(cam->Position - LessonBlendingData::windows[i]);
            sorted[distance] = LessonBlendingData::windows[i];
        }

        // render
        // ------
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // draw objects;
        LessonBlendingData::blendingShader->use();
        glm::mat4 projection = glm::perspective(glm::radians(cam->Zoom), (float)LessonBlendingData::SCR_WIDTH / (float)LessonBlendingData::SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = cam->GetViewMatrix();
        glm::mat4 model = glm::mat4(1.0f);
        LessonBlendingData::blendingShader->set4Matrix("projection", projection);
        LessonBlendingData::blendingShader->set4Matrix("view", view);

        // cubes
        glBindVertexArray(LessonBlendingData::cubeVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, LessonBlendingData::cubeTexture);
        model = glm::translate(model, glm::vec3(-1.0f, 0.0f, -1.0f));
        LessonBlendingData::blendingShader->set4Matrix("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(2.0f, 0.0f, 0.0f));
        LessonBlendingData::blendingShader->set4Matrix("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // floor
        glBindVertexArray(LessonBlendingData::planeVAO);
        glBindTexture(GL_TEXTURE_2D, LessonBlendingData::floorTexture);
        model = glm::mat4(1.0f);
        LessonBlendingData::blendingShader->set4Matrix("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // windows (from furthest to nearest)
        glBindVertexArray(LessonBlendingData::transparentVAO);
        glBindTexture(GL_TEXTURE_2D, LessonBlendingData::transparentTexture);

        for(std::map<float, glm::vec3>::reverse_iterator it = sorted.rbegin(); it != sorted.rend(); it++)
        {
            model = glm::mat4(1.0f);
            model = glm::translate(model, it->second);
            LessonBlendingData::blendingShader->set4Matrix("model", model);
            glDrawArrays(GL_TRIANGLES, 0 ,6);
        }
    }


    static void clean()
    {
        glDeleteVertexArrays(1, &LessonBlendingData::cubeVAO);
        glDeleteVertexArrays(1, &LessonBlendingData::planeVAO);
        glDeleteVertexArrays(1, &LessonBlendingData::transparentVAO);

        glDeleteBuffers(1, &LessonBlendingData::cubeVBO);
        glDeleteBuffers(1, &LessonBlendingData::planeVBO);
        glDeleteBuffers(1, &LessonBlendingData::transparentVBO);

        delete LessonBlendingData::blendingShader;
    }


    static unsigned int LoadTexture(char const* path)
    {
        unsigned int textureID;
        glGenTextures(1, &textureID);

        int width, height, nrComponents;
        unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
        if(data)
        {
            GLenum format;
            if (nrComponents == 1)
                format = GL_RED;
            else if(nrComponents == 3)
                format = GL_RGB;
            else if(nrComponents == 4)
                format = GL_RGBA;

            glBindTexture(GL_TEXTURE_2D, textureID);
            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
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



#endif /* __LESSONS_H__ */