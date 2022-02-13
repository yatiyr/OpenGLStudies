#ifndef __SPHERE_H__
#define __SPHERE_H__

#include <iostream>

#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <glad/glad.h>

class Sphere
{
public:
    Sphere(float radius=1.0f, int sectorCount=64, int stackCount=64);
    ~Sphere() {}

    float getRadius()           const {return radius;}
    float getSectorCount()      const {return sectorCount;}
    float getStackCount()       const {return stackCount;}
    
    void set(float radius, int sectorCount, int stackCount);
    void setRadius(float radius);
    void setSectorCount(int sectorCount);
    void setStackCount(int stackCount);

    // vertex data
    unsigned int getVertexCount() const     {return (unsigned int) vertices.size() / 3;}
    unsigned int getNormalCount() const     {return (unsigned int) normals.size() / 3;}
    unsigned int getBitangentCount() const  {return (unsigned int) bitangents.size() / 3;}
    unsigned int getTangentCount() const    {return (unsigned int) tangents.size() / 3;}
    unsigned int getTexCoordCount() const   {return (unsigned int) texCoords.size() / 2;}
    unsigned int getIndexCount() const      {return (unsigned int)indices.size();}
    unsigned int getTriangleCount() const   {return getIndexCount() / 3;}
    unsigned int getVertexSize() const      {return (unsigned int)vertices.size() * sizeof(float);}
    unsigned int getNormalSize() const      {return (unsigned int)normals.size() * sizeof(float);}
    unsigned int getTangentSize() const     {return (unsigned int)tangents.size() * sizeof(float);}
    unsigned int getBitangentSize() const   {return (unsigned int)bitangents.size() * sizeof(float);}
    unsigned int getTexCoordSize() const    {return (unsigned int)texCoords.size() * sizeof(float);}
    unsigned int getIndexSize() const       {return (unsigned int)indices.size() * sizeof(unsigned int);}

    const std::vector<glm::vec3> getVertices()   const {return vertices;}
    const std::vector<glm::vec3> getNormals()    const {return normals;}
    const std::vector<glm::vec3> getTangents()   const {return tangents;}
    const std::vector<glm::vec3> getBitangents() const {return bitangents;}
    const std::vector<glm::vec2> getTexCoords()  const {return texCoords;}

    const unsigned int* getIndices() const {return indices.data();}


    int getInterleavedStride() const {return interleavedStride;}

    void draw() const;

    void printSelf() const;

private:

    void buildVertices();
    void buildInterleavedVertices();
    void setupArrayBuffer();
    void setupMesh();
    void clearArrays();

    void addVertex(float x, float y, float z);
    void addNormal(float x, float y, float z);
    void addTangent(float x, float y, float z);
    void addBitangent(float x, float y, float z);
    void addTexCoord(float s, float t);
    void addIndices(unsigned int i1, unsigned int i2, unsigned int i3);

    glm::vec3 computeFaceNormal(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3);


    std::vector<glm::vec3> calcTangentBitangents(glm::vec3 pos1, glm::vec3 pos2, glm::vec3 pos3,
                                                 glm::vec2 uv1,  glm::vec2 uv2,  glm::vec2 uv3)
    {
        std::vector<glm::vec3> result;

        glm::vec3 tangent, bitangent;

        glm::vec3 edge1 = pos2 - pos1;
        glm::vec3 edge2 = pos3 - pos1;

        glm::vec2 deltaUV1 = uv2 - uv1;
        glm::vec2 deltaUV2 = uv3 - uv1;

        float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

        tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
        tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
        tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);

        bitangent.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
        bitangent.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
        bitangent.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);

        result.push_back(tangent);
        result.push_back(bitangent);

        return result;        
    }
                                                   
    // member variables
    float radius;
    int sectorCount;
    int stackCount;
    
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec3> tangents;
    std::vector<glm::vec3> bitangents;
    std::vector<glm::vec2> texCoords;
    std::vector<unsigned int> indices;
    
    // interleaved
    std::vector<float> arrayBuffer;
    int interleavedStride;


    // Array object and buffers
    unsigned int VAO = 0, VBO = 0, EBO = 0;

    
};






#endif