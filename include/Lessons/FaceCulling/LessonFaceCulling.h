#ifndef __LESSON_FACE_CULLING_H__
#define __LESSON_FACE_CULLING_H__

#include <Lessons/LessonEssentials.h>
#include <Lessons/FaceCulling/LessonFaceCullingData.h>

class LessonFaceCulling
{
public:

    static void initialConf()
    {
        // flip loaded textures vertically
        setVerticalFlip(false);

        // Global opengl state specific to this lesson
        glEnable(GL_CULL_FACE);
        // discard back facing triangles
        glCullFace(GL_BACK);

        // front face is counter clock wise
        glFrontFace(GL_CW);

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        LessonFaceCullingData::faceCullingShader = new ShaderProgram("shaders/blending/blending.vert",
                                                                     "shaders/blending/blending.frag");

        // perepare LessonBlendingData::cubeVAO
        glGenVertexArrays(1, &LessonFaceCullingData::cubeVAO);
        glGenBuffers(1, &LessonFaceCullingData::cubeVBO);
        glBindVertexArray(LessonFaceCullingData::cubeVAO);
        glBindBuffer(GL_ARRAY_BUFFER, LessonFaceCullingData::cubeVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(LessonFaceCullingData::verticesCube), &LessonFaceCullingData::verticesCube, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3*sizeof(float)));


        // load texture
        std::string cubeTexturePath = std::string(ROOT_DIR) + "assets/textures/blend/marble.png";

        LessonFaceCullingData::cubeTexture = LoadTexture(cubeTexturePath.c_str());

        // use shader
        LessonFaceCullingData::faceCullingShader->use();
        LessonFaceCullingData::faceCullingShader->setInt("texture", 0);
    }

    static void draw(Camera *cam)
    {
        glClearColor(0.9f, 0.9f, 0.9f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // draw object
        LessonFaceCullingData::faceCullingShader->use();
        glm::mat4 projection = glm::perspective(glm::radians(cam->Zoom), (float)LessonFaceCullingData::SCR_WIDTH / (float)LessonFaceCullingData::SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = cam->GetViewMatrix();
        glm::mat4 model = glm::mat4(1.0f);
        LessonFaceCullingData::faceCullingShader->set4Matrix("projection", projection);
        LessonFaceCullingData::faceCullingShader->set4Matrix("view", view);

        glBindVertexArray(LessonFaceCullingData::cubeVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, LessonFaceCullingData::cubeTexture);
        LessonFaceCullingData::faceCullingShader->set4Matrix("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }

    static void clean()
    {
        glDeleteVertexArrays(1, &LessonFaceCullingData::cubeVAO);
        
        glDeleteBuffers(1, &LessonFaceCullingData::cubeVBO);

        delete LessonFaceCullingData::faceCullingShader;
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



#endif