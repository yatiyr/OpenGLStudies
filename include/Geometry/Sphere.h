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
    Sphere(float radius, bool smooth);
    ~Sphere();

protected:

// -------------------------------------------------- //
//                  MEMBER VARIABLES                  //
    float radius;
    bool smooth;

    std::vector<glm::vec3>    vertices;
    std::vector<glm::vec3>    normals;
    std::vector<glm::vec3>    tangents;
    std::vector<glm::vec3>    bitangents;
    std::vector<glm::vec2>    texCoords;

    // Array object and buffers
    unsigned int VAO = 0, VBO = 0, EBO = 0;
    std::vector<float>        arrayBuffer;
    std::vector<unsigned int> indices;

// -------------------------------------------------- //
//                  VIRTUAL FUNCTIONS                 //

    virtual void BuildVertices() = 0;
    virtual void BuildVerticesFlat() = 0;

// -------------------------------------------------- //
//                  SHARED FUNCTIONS                  //

    void SetupArrayBuffer();
    void SetupMesh();

    void SetupTangentBitangents();

    void AddIndices(unsigned int i1, unsigned int i2, unsigned int i3);

    void ClearArrays();

    unsigned int GetVertexCount() const;
    unsigned int GetNormalCount() const;
    unsigned int GetBitangentCount() const;
    unsigned int GetTangentCount() const;
    unsigned int GetTexCoordCount() const;
    unsigned int GetIndexCount() const;
    unsigned int GetTriangleCount() const;
    unsigned int GetVertexSize() const;
    unsigned int GetNormalSize() const;
    unsigned int GetTangentSize() const;
    unsigned int GetBitangentSize() const;
    unsigned int GetTexCoordSize() const;
    unsigned int GetIndexSize() const;
    float        GetRadius() const;

    const std::vector<glm::vec3> GetVertices()   const;
    const std::vector<glm::vec3> GetNormals()    const;
    const std::vector<glm::vec3> GetTangents()   const;
    const std::vector<glm::vec3> GetBitangents() const;
    const std::vector<glm::vec2> GetTexCoords()  const;
    const unsigned int* GetIndices() const;

// -------------------------------------------------- //
//                  UTILITY FUNCTIONS                 //

    std::vector<glm::vec3> CalcTangentBitangents(glm::vec3 pos1,
                                                 glm::vec3 pos2,
                                                 glm::vec3 pos3,
                                                 glm::vec2 uv1,
                                                 glm::vec2 uv2,
                                                 glm::vec2 uv3);

    glm::vec3 ComputeFaceNormal(const glm::vec3& v1, 
                                const glm::vec3& v2,
                                const glm::vec3& v3);                                                 

// -------------------------------------------------- //

public:
    void Draw() const;

    void SetSmooth(bool smooth);
};



#endif