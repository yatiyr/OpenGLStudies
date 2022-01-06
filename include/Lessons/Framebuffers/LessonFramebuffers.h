#ifndef __LESSON_FRAMEBUFFERS_H__
#define __LESSON_FRAMEBUFFERS_H__

#include <Lessons/LessonEssentials.h>
#include <Lessons/Framebuffers/LessonFramebuffersData.h>

class LessonFramebuffers
{
public:

    static void initialConf()
    {
        // flip loaded texture vertiaclly
        setVerticalFlip(false);

        // Global opengl state
        glEnable(GL_DEPTH_TEST);

        LessonFramebuffersData::shader = new ShaderProgram("shaders/framebuffers/shader.vert",
                                                       "shaders/framebuffers/shader.frag");
    
        LessonFramebuffersData::screenShader = new ShaderProgram("shaders/framebuffers/screenShader.vert",
                                                             "shaders/framebuffers/screenShader.frag");

        // cube VAO
        glGenVertexArrays(1, &LessonFramebuffersData::cubeVAO);
        glGenBuffers(1, &LessonFramebuffersData::cubeVBO);
        glBindVertexArray(LessonFramebuffersData::cubeVAO);
        glBindBuffer(GL_ARRAY_BUFFER, LessonFramebuffersData::cubeVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(LessonFramebuffersData::verticesCube), &LessonFramebuffersData::verticesCube, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),  (void*)(3 * sizeof(float)));

        // plane VAO
        glGenVertexArrays(1, &LessonFramebuffersData::planeVAO);
        glGenBuffers(1, &LessonFramebuffersData::planeVBO);
        glBindVertexArray(LessonFramebuffersData::planeVAO);
        glBindBuffer(GL_ARRAY_BUFFER, LessonFramebuffersData::planeVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(LessonFramebuffersData::planeVertices), &LessonFramebuffersData::planeVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

        // screen quad VAO
        glGenVertexArrays(1, &LessonFramebuffersData::quadVAO);
        glGenBuffers(1, &LessonFramebuffersData::quadVBO);
        glBindVertexArray(LessonFramebuffersData::quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, LessonFramebuffersData::quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(LessonFramebuffersData::quadVertices), &LessonFramebuffersData::quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

        // load textures
        std::string cubeTexturePath = std::string(ROOT_DIR) + "assets/textures/blend/marble.png";
        std::string floorTexturePath = std::string(ROOT_DIR) + "assets/textures/blend/metal.png";        

        LessonFramebuffersData::cubeTexture = LoadTexture(cubeTexturePath.c_str());
        LessonFramebuffersData::floorTexture = LoadTexture(floorTexturePath.c_str());

        // shader configuration
        LessonFramebuffersData::shader->use();
        LessonFramebuffersData::shader->setInt("texture1", 0);

        LessonFramebuffersData::screenShader->use();
        LessonFramebuffersData::screenShader->setInt("screenTexture", 0);


        // Framebuffer configuration
        glGenFramebuffers(1, &LessonFramebuffersData::framebuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, LessonFramebuffersData::framebuffer);

        // create a color attachment texture
        glGenTextures(1, &LessonFramebuffersData::textureColorBuffer);
        glBindTexture(GL_TEXTURE_2D, LessonFramebuffersData::textureColorBuffer);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, LessonFramebuffersData::SCR_WIDTH, LessonFramebuffersData::SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);        
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, LessonFramebuffersData::textureColorBuffer, 0);
        // create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
        glGenRenderbuffers(1, &LessonFramebuffersData::rbo);
        glBindRenderbuffer(GL_RENDERBUFFER, LessonFramebuffersData::rbo);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, LessonFramebuffersData::SCR_WIDTH, LessonFramebuffersData::SCR_HEIGHT);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, LessonFramebuffersData::rbo);
        // now that we actually created the framebuffer and added all attachments, we want to check if it is actually complete now
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // for drawing as wireframe
        // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        
    }

    static void draw(Camera *cam)
    {
        // render
        // ------
        // bind to framebuffer and draw cene as we normally would to color texture
        glBindFramebuffer(GL_FRAMEBUFFER, LessonFramebuffersData::framebuffer);
        glEnable(GL_DEPTH_TEST); // enable depth testing (is disabled for rendering screen-space quad)

        // make sure we clear the framebuffer's content
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        LessonFramebuffersData::shader->use();
        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = cam->GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(cam->Zoom), (float)LessonFramebuffersData::SCR_WIDTH / (float)LessonFramebuffersData::SCR_HEIGHT, 0.1f, 100.0f);
        LessonFramebuffersData::shader->set4Matrix("view", view);
        LessonFramebuffersData::shader->set4Matrix("projection", projection);

        // cubes
        glBindVertexArray(LessonFramebuffersData::cubeVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, LessonFramebuffersData::cubeTexture);
        model = glm::translate(model, glm::vec3(-1.0f, 0.0f, -1.0f));
        LessonFramebuffersData::shader->set4Matrix("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(2.0f, 0.0f, 0.0f));
        LessonFramebuffersData::shader->set4Matrix("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // floor
        glBindVertexArray(LessonFramebuffersData::planeVAO);
        glBindTexture(GL_TEXTURE_2D, LessonFramebuffersData::floorTexture);
        LessonFramebuffersData::shader->set4Matrix("model", glm::mat4(1.0f));
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);

        // now bind back to default framebuffer and draw a quad plane with the attached framebuffer color texture
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glDisable(GL_DEPTH_TEST);
        // clear all relevant buffers
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);


        LessonFramebuffersData::screenShader->use();
        glBindVertexArray(LessonFramebuffersData::quadVAO); 
        glBindTexture(GL_TEXTURE_2D, LessonFramebuffersData::textureColorBuffer);               
        glDrawArrays(GL_TRIANGLES, 0, 6);         
   
    }

    static void clean()
    {
        glDeleteVertexArrays(1, &LessonFramebuffersData::cubeVAO);
        glDeleteVertexArrays(1, &LessonFramebuffersData::planeVAO);
        glDeleteVertexArrays(1, &LessonFramebuffersData::quadVAO);
        glDeleteBuffers(1, &LessonFramebuffersData::cubeVBO);
        glDeleteBuffers(1, &LessonFramebuffersData::planeVBO);
        glDeleteBuffers(1, &LessonFramebuffersData::quadVBO);


        delete LessonFramebuffersData::shader;
        delete LessonFramebuffersData::screenShader;
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
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
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