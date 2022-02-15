#ifndef __PBR_H__
#define __PBR_H__


#include <Lessons/LessonEssentials.h>
#include <Lessons/Pbr/PbrData.h>
#include <Geometry/RadialSphere.h>
#include <Geometry/Icosphere.h>

class Pbr
{
public:
    static void initialConf()
    {
        // configure global opengl state
        // -----------------------------
        glEnable(GL_DEPTH_TEST);

        // build and compile shader
        PbrData::shader = new ShaderProgram("shaders/pbr/pbr.vert", "shaders/pbr/pbr.frag");

        // load textures
        std::string albedoPath    = std::string(ROOT_DIR) + "assets/textures/pbr/rustediron2_basecolor.png";
        std::string normalPath    = std::string(ROOT_DIR) + "assets/textures/pbr/rustediron2_normal.png";
        std::string metallicPath  = std::string(ROOT_DIR) + "assets/textures/pbr/rustediron2_metallic.png";
        std::string roughnessPath = std::string(ROOT_DIR) + "assets/textures/pbr/rustediron2_roughness.png";
        std::string aoPath        = std::string(ROOT_DIR) + "assets/textures/pbr/rustediron2_ao.png";

        PbrData::albedoMap    = LoadTexture(albedoPath.c_str());
        PbrData::normalMap    = LoadTexture(normalPath.c_str());
        PbrData::metallicMap  = LoadTexture(metallicPath.c_str());
        PbrData::roughnessMap = LoadTexture(roughnessPath.c_str());
        PbrData::aoMap        = LoadTexture(aoPath.c_str());        

        // smooth shading is on for both sphere types
        PbrData::sphere = new RadialSphere(1, 64, 64, true);
        PbrData::sphere2 = new Icosphere(1, 4, true);

        PbrData::shader->use();
        PbrData::shader->setVec3("albedo", 0.5f, 0.0f, 0.0f);
        PbrData::shader->setFloat("ao", 1.0f);

        PbrData::shader->setInt("albedoMap", 0);
        PbrData::shader->setInt("normalMap", 1);
        PbrData::shader->setInt("metallicMap", 2);
        PbrData::shader->setInt("roughnessMap", 3);
        PbrData::shader->setInt("aoMap", 4);
    }

    static void draw(Camera* cam, float currentFrame)
    {
        // render
        // -----------
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        PbrData::shader->use();
        glm::mat4 view = cam->GetViewMatrix();
        PbrData::projection = glm::perspective(glm::radians(cam->Zoom), (float)PbrData::SCR_WIDTH / (float)PbrData::SCR_HEIGHT, 0.1f, 100.0f);
        PbrData::shader->set4Matrix("projection", PbrData::projection);
        PbrData::shader->set4Matrix("view", view);
        PbrData::shader->setVec3("camPos", cam->Position);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, PbrData::albedoMap);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, PbrData::normalMap);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, PbrData::metallicMap);
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, PbrData::roughnessMap);
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, PbrData::aoMap);

        // render rows*column number of spheres with varying metallic/roughness values scaled by rows and columns respectively
        glm::mat4 model = glm::mat4(1.0f);
        for(int row=0; row<PbrData::nrRows; row++)
        {
            PbrData::shader->setFloat("metallic", (float)row / (float)PbrData::nrRows);
            for(int col=0; col<PbrData::nrColumns; col++)
            {
                // we clamp the roughness to 0.05 - 1.0 as perfectly smooth surfaces tent to look a bit off
                // on direct lighting
                PbrData::shader->setFloat("roughness", glm::clamp((float)col / (float)PbrData::nrColumns, 0.05f, 1.0f));

                model = glm::mat4(1.0f);
                model = glm::translate(model, glm::vec3(
                    (col - (PbrData::nrColumns / 2)) * PbrData::spacing,
                    (row - (PbrData::nrRows / 2)) * PbrData::spacing,
                    0.0f
                ));
                PbrData::shader->set4Matrix("model", model);
                PbrData::sphere->Draw();
            }
        }

        // render light source
        for(unsigned int i=0; i< sizeof(PbrData::lightPositions) / sizeof(PbrData::lightPositions[0]); i++)
        {
            glm::vec3 newPos = PbrData::lightPositions[i] + glm::vec3(sin(currentFrame * 3.0) * 10.0, 0.0, 0.0);
            //newPos = PbrData::lightPositions[i];
            PbrData::shader->setVec3("lightPositions[" + std::to_string(i) + "]", newPos);
            PbrData::shader->setVec3("lightColors[" + std::to_string(i) + "]", PbrData::lightColors[i]);

            model = glm::mat4(1.0f);
            model = glm::translate(model, newPos);
            model = glm::scale(model, glm::vec3(0.5f));
            PbrData::shader->set4Matrix("model", model);
            PbrData::sphere->Draw();
        }

    }

    static void clean()
    {
        delete PbrData::sphere;
        delete PbrData::sphere2;
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