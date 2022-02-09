#ifndef __MESH_H__
#define __MESH_H__

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <string>


#include <ShaderProgram.h>

#define MAX_BONE_INFLUENCE 4

enum TextureType
{
    texture_diffuse  = 1,
    texture_specular = 2,
    texture_normal   = 3,
    texture_height   = 4
};

// Holds vertex data of the mesh
struct Vertex
{
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
    glm::vec3 Tangent;
    glm::vec3 Bitangent;

        // bone indexes which will influence this vertex
        int m_BoneIDs[MAX_BONE_INFLUENCE];

        // weights from each bone
        float m_Weights[MAX_BONE_INFLUENCE];

};

// Holds texture data of the mesh
struct Texture
{
    unsigned int id;
    TextureType type;
    std::string path;
};

// We abstract rendering with mesh class
class Mesh
{
public:
    std::vector<Vertex>       vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture>      textures;
    unsigned int VAO;

    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures)
    {
        this->vertices = vertices;
        this->indices  = indices;
        this->textures = textures;

        setupMesh();
    }
    void Draw(ShaderProgram &shaderProgram)
    {
        unsigned int nDiffuse  = 1;
        unsigned int nSpecular = 1;
        unsigned int nNormal   = 1;
        unsigned int nHeight   = 1;

        for(unsigned int i=0; i<textures.size(); i++)
        {
            // activate proper texture unit before binding
            glActiveTexture(GL_TEXTURE0 + i);
            std::string number;
            TextureType type = textures[i].type;
            if(type == TextureType::texture_diffuse)
            {
                number = std::to_string(nDiffuse++);
                glUniform1f(glGetUniformLocation(shaderProgram.id, ("material.texture_diffuse" + number).c_str()), i);
            }
            else if(type == TextureType::texture_specular)
            {
                number = std::to_string(nSpecular++);
                glUniform1f(glGetUniformLocation(shaderProgram.id, ("material.texture_specular" + number).c_str()), i);
            }
            else if(type == TextureType::texture_normal)
            {
                number = std::to_string(nNormal++);
                glUniform1f(glGetUniformLocation(shaderProgram.id, ("material.texture_normal" + number).c_str()), i);
            }
            else if(type == TextureType::texture_height)
            {
                number = std::to_string(nHeight++);
                glUniform1f(glGetUniformLocation(shaderProgram.id, ("material.texture_height" + number).c_str()), i);
            }
            // Ileride roughness falan eklersing

            glBindTexture(GL_TEXTURE_2D, textures[i].id);         
        }   

        // draw mesh
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
     
        glActiveTexture(GL_TEXTURE0);     
    }

    void DrawNoTexture(ShaderProgram &shaderProgram, unsigned int cubemap)
    {
        glBindVertexArray(VAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap);        
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

private:
    unsigned int VBO, EBO;

    void setupMesh()
    {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

        // position data of vertices
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

        // normal data of vertices
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, Normal));

        // texture coordinates of vertices
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

        // vertex tangent of vertices
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));

        // vertex bitangent of vertices
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));

            // id
            glEnableVertexAttribArray(5);
            glVertexAttribIPointer(5, 4, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, m_BoneIDs));

            // weights
            glEnableVertexAttribArray(6);
            glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_Weights));

        // unbind vertex array object
        glBindVertexArray(0);
    }
};
#endif