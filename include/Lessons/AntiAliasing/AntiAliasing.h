#ifndef __AntiAliasing_H__
#define __AntiAliasing_H__

#include <Lessons/LessonEssentials.h>
#include <Lessons/AntiAliasing/AntiAliasingData.h>
#include <Model.h>


class AntiAliasing
{
public:
    static void initialConf()
    {
        glEnable(GL_DEPTH_TEST);

        AntiAliasingData::shader = new ShaderProgram("shaders/antialiasing/antialiasing.vert",
                                                     "shaders/antialiasing/antialiasing.frag");
        AntiAliasingData::screenShader = new ShaderProgram("shaders/antialiasing/post.vert",
                                                           "shaders/antialiasing/post.frag");


        // setup cube VAO
        unsigned int cubeVBO;
        glGenVertexArrays(1, &AntiAliasingData::cubeVAO);
        glGenBuffers(1, &cubeVBO);
        glBindVertexArray(AntiAliasingData::cubeVAO);
        glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(AntiAliasingData::cubeVertices), &AntiAliasingData::cubeVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

        // setup screen VAO
        unsigned int quadVBO;
        glGenVertexArrays(1, &AntiAliasingData::quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(AntiAliasingData::quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(AntiAliasingData::quadVertices), &AntiAliasingData::quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));


        // configure MSAA framebuffer
        glGenFramebuffers(1, &AntiAliasingData::framebuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, AntiAliasingData::framebuffer);
        // create multisampled color attachment texture
        unsigned int textureColorBufferMultiSampled;
        glGenTextures(1, &textureColorBufferMultiSampled);
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, textureColorBufferMultiSampled);
        glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGB, AntiAliasingData::SCR_WIDTH, AntiAliasingData::SCR_HEIGHT, GL_TRUE);
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, textureColorBufferMultiSampled, 0);
        // create a (also multisampled) renderbuffer object for depth and stencil attachments
        unsigned int rbo;
        glGenRenderbuffers(1, &rbo);
        glBindRenderbuffer(GL_RENDERBUFFER, rbo);
        glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8, AntiAliasingData::SCR_WIDTH, AntiAliasingData::SCR_HEIGHT);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

        if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // configure second post-processing framebuffer
        glGenFramebuffers(1, &AntiAliasingData::intermediateFBO);
        glBindFramebuffer(GL_FRAMEBUFFER, AntiAliasingData::intermediateFBO);
        // create a color attachment texture
        glGenTextures(1, &AntiAliasingData::screenTexture);
        glBindTexture(GL_TEXTURE_2D, AntiAliasingData::screenTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, AntiAliasingData::SCR_WIDTH, AntiAliasingData::SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, AntiAliasingData::screenTexture, 0);
        // we only need color buffer
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "ERROR:FRAMEBUFFER:: Intermediate framebuffer is not complete!" << std::endl;
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        AntiAliasingData::screenShader->use();
        AntiAliasingData::screenShader->setInt("screenTexture", 0);

    }   

    static void draw(Camera *cam)
    {
        // render
        // ------
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // 1. draw scene as normal in multisampled buffers
        glBindFramebuffer(GL_FRAMEBUFFER, AntiAliasingData::framebuffer);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);

        // set transformation matrices
        AntiAliasingData::shader->use();
        glm::mat4 projection = glm::perspective(glm::radians(cam->Zoom), (float)AntiAliasingData::SCR_WIDTH / (float)AntiAliasingData::SCR_HEIGHT, 0.1f, 1000.0f);
        AntiAliasingData::shader->set4Matrix("projection", projection);
        AntiAliasingData::shader->set4Matrix("view", cam->GetViewMatrix());
        AntiAliasingData::shader->set4Matrix("model", glm::mat4(1.0f));

        glBindVertexArray(AntiAliasingData::cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // 2. now blit multisampled buffer(s) to normal colorbuffer of intermediate FBO. Image is stored in screenTexture
        glBindFramebuffer(GL_READ_FRAMEBUFFER, AntiAliasingData::framebuffer);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, AntiAliasingData::intermediateFBO);
        glBlitFramebuffer(0, 0, AntiAliasingData::SCR_WIDTH, AntiAliasingData::SCR_HEIGHT, 0, 0, AntiAliasingData::SCR_WIDTH, AntiAliasingData::SCR_HEIGHT, GL_COLOR_BUFFER_BIT, GL_NEAREST);

        // 3. now render quad with scene's visuals as its texture image
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glDisable(GL_DEPTH_TEST);

        // draw Screen quad
        AntiAliasingData::screenShader->use();
        glBindVertexArray(AntiAliasingData::quadVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, AntiAliasingData::screenTexture); // use the now resolves color attachment as the quad's texture
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }

    static void clean()
    {

        glDeleteVertexArrays(1, &AntiAliasingData::cubeVAO);
        glDeleteVertexArrays(1, &AntiAliasingData::quadVAO);

        delete AntiAliasingData::shader;
        delete AntiAliasingData::screenShader;


    }
};


#endif