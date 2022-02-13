#ifndef __PARALLAX_MAPPING_H__
#define __PARALLAX_MAPPING_H__

#include <Lessons/LessonEssentials.h>
#include <Lessons/ParallaxMapping/ParallaxMappingData.h>

class ParallaxMapping
{
public:
    static void initialConf()
    {
        // configure global opengl state
        // -----------------------------
        glEnable(GL_DEPTH_TEST);

        // build and compile shader
        ParallaxMappingData::shader = new ShaderProgram("shaders/parallaxMapping/parallaxMapping.vert",
                                                        "shaders/parallaxMapping/parallaxMapping.frag");

        std::string diffuseTexturePath = std::string(ROOT_DIR) + "assets/textures/parallaxMapping/cobblestone_albedo.png";
        std::string normalTexturePath = std::string(ROOT_DIR) + "assets/textures/parallaxMapping/cobblestone_normal.png";
        std::string heightTexturePath = std::string(ROOT_DIR) + "assets/textures/parallaxMapping/cobblestone_height.png";

        ParallaxMappingData::diffuseMap = LoadTexture(diffuseTexturePath.c_str());
        ParallaxMappingData::normalMap = LoadTexture(normalTexturePath.c_str());
        ParallaxMappingData::heightMap = LoadTexture(heightTexturePath.c_str());

        ParallaxMappingData::sphere = new Sphere();

        ParallaxMappingData::shader->use();
        ParallaxMappingData::shader->setInt("diffuseMap", 0);
        ParallaxMappingData::shader->setInt("normalMap", 1);
        ParallaxMappingData::shader->setInt("heightMap", 2);

    }

    static void draw(Camera *cam, float currentFrame)
    {
        // render
        // ------
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // configure view/projection matrices
        glm::mat4 projection = glm::perspective(glm::radians(cam->Zoom), (float)ParallaxMappingData::SCR_WIDTH / (float)ParallaxMappingData::SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = cam->GetViewMatrix();
        ParallaxMappingData::shader->use();
        ParallaxMappingData::shader->set4Matrix("projection", projection);
        ParallaxMappingData::shader->set4Matrix("view", view);

        // render parallax-mapped quad
        glm::mat4 model = glm::mat4(1.0f);
        //model = glm::rotate(model, glm::radians(currentFrame * -0.1f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
        model = glm::scale(model, glm::vec3(5.0,5.0,5.0));
        ParallaxMappingData::lightPos = glm::vec3(15.0 * sin(currentFrame * 0.4), 1.0, 15.0 * cos(currentFrame * 0.4) + 6);
        ParallaxMappingData::shader->set4Matrix("model", model);
        ParallaxMappingData::shader->setVec3("viewPos", cam->Position);
        ParallaxMappingData::shader->setVec3("lightPos", ParallaxMappingData::lightPos);
        ParallaxMappingData::shader->setFloat("heightScale", ParallaxMappingData::heightScale);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, ParallaxMappingData::diffuseMap);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, ParallaxMappingData::normalMap);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, ParallaxMappingData::heightMap);
        renderQuad();

        //model = glm::translate(model, glm::vec3(10.0f, 5.0f, 5.0f));
        model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
        ParallaxMappingData::shader->set4Matrix("model", model);
        ParallaxMappingData::sphere->draw();

        // render light source
        model = glm::mat4(1.0f);
        model = glm::translate(model, ParallaxMappingData::lightPos);
        model = glm::scale(model, glm::vec3(0.1f));
        ParallaxMappingData::shader->set4Matrix("model", model);
        renderQuad();
    }

    static void clean()
    {
        glDeleteVertexArrays(1, &ParallaxMappingData::quadVAO);
        delete ParallaxMappingData::shader;
    }

    static void renderQuad()
    {
        if (ParallaxMappingData::quadVAO == 0)
        {
            // positions
            glm::vec3 pos1(-1.0f,  1.0f, 0.0f);
            glm::vec3 pos2(-1.0f, -1.0f, 0.0f);
            glm::vec3 pos3( 1.0f, -1.0f, 0.0f);
            glm::vec3 pos4( 1.0f,  1.0f, 0.0f);
            // texture coordinates
            glm::vec2 uv1(0.0f, 1.0f);
            glm::vec2 uv2(0.0f, 0.0f);
            glm::vec2 uv3(1.0f, 0.0f);
            glm::vec2 uv4(1.0f, 1.0f);
            // normal vector
            glm::vec3 nm(0.0f, 0.0f, 1.0f);

            // calculate tangent/bitangent vectors of both triangles
            glm::vec3 tangent1, bitangent1;
            glm::vec3 tangent2, bitangent2;
            // triangle 1
            // ----------
            glm::vec3 edge1 = pos2 - pos1;
            glm::vec3 edge2 = pos3 - pos1;
            glm::vec2 deltaUV1 = uv2 - uv1;
            glm::vec2 deltaUV2 = uv3 - uv1;

            float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

            tangent1.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
            tangent1.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
            tangent1.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);

            bitangent1.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
            bitangent1.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
            bitangent1.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);

            // triangle 2
            // ----------
            edge1 = pos3 - pos1;
            edge2 = pos4 - pos1;
            deltaUV1 = uv3 - uv1;
            deltaUV2 = uv4 - uv1;

            f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

            tangent2.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
            tangent2.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
            tangent2.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);


            bitangent2.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
            bitangent2.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
            bitangent2.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);

            float quadVertices[] = {
                // positions            // normal         // texcoords  // tangent                          // bitangent
                pos1.x, pos1.y, pos1.z, nm.x, nm.y, nm.z, uv1.x, uv1.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,
                pos2.x, pos2.y, pos2.z, nm.x, nm.y, nm.z, uv2.x, uv2.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,
                pos3.x, pos3.y, pos3.z, nm.x, nm.y, nm.z, uv3.x, uv3.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,

                pos1.x, pos1.y, pos1.z, nm.x, nm.y, nm.z, uv1.x, uv1.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z,
                pos3.x, pos3.y, pos3.z, nm.x, nm.y, nm.z, uv3.x, uv3.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z,
                pos4.x, pos4.y, pos4.z, nm.x, nm.y, nm.z, uv4.x, uv4.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z
            };

            // configure plane VAO
            glGenVertexArrays(1, &ParallaxMappingData::quadVAO);
            glGenBuffers(1, &ParallaxMappingData::quadVBO);
            glBindVertexArray(ParallaxMappingData::quadVAO);
            glBindBuffer(GL_ARRAY_BUFFER, ParallaxMappingData::quadVBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(3 * sizeof(float)));
            glEnableVertexAttribArray(2);
            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(6 * sizeof(float)));
            glEnableVertexAttribArray(3);
            glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(8 * sizeof(float)));
            glEnableVertexAttribArray(4);
            glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(11 * sizeof(float)));

        }

        glBindVertexArray(ParallaxMappingData::quadVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);
    }

    static void processInput(GLFWwindow* window)
    {
        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        {
            if (ParallaxMappingData::heightScale > 0.0f)
                ParallaxMappingData::heightScale -= 0.0005f;
            else
                ParallaxMappingData::heightScale = 0.0f;
        }
        else if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        {
            if (ParallaxMappingData::heightScale < 1.0f)
                ParallaxMappingData::heightScale += 0.0005f;
            else
                ParallaxMappingData::heightScale = 1.0f;
        }
    }    

    static unsigned int LoadTexture(char const* path)
    {
        unsigned int textureID;
        glGenTextures(1, &textureID);

        int width, height, nrComponents;
        unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
        if (data)
        {
            GLenum format;
            if (nrComponents == 1)
                format = GL_RED;
            else if (nrComponents == 3)
                format = GL_RGB;
            else if (nrComponents == 4)
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