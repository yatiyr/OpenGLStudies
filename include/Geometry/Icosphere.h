#ifndef __Icosphere_H__
#define __Icosphere_H__

#include <Geometry/Sphere.h>

#include <iostream>

#include <vector>
#include <map>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <glad/glad.h>

class Icosphere : public Sphere
{
public:
    Icosphere(float radius=1.0f, int subdivision=1, bool smooth = true);
    ~Icosphere() {}

    int GetSubdivision()      const {return subdivision;}
    
    void Set(float radius, int subdivision);
    void SetRadius(float radius);
    void SetSubdivision(int subdivision);
    void UpdateRadius();
    void PrintSelf() const;

private:

    virtual void BuildVertices();
    virtual void BuildVerticesFlat();
    
    void SubdivideVertices();


    std::vector<glm::vec3> ComputeIcosahedronVertices();

    void ComputeVertexNormal(const glm::vec3& vertex, glm::vec3& normal);
    void ComputeHalfVertex(const glm::vec3& v1, const glm::vec3& v2, float length, glm::vec3& newVertex);
    void ComputeHalfTexCoord(const glm::vec2& t1, const glm::vec2& t2, glm::vec2& newTexCoord);

    float ComputeScaleForLength(glm::vec3 vertex, float length);
    bool IsSharedTexCoord(const glm::vec2& t);
    bool IsOnLineSegment(const glm::vec2& a, const glm::vec2& b, const glm::vec2& c);

    unsigned int AddSubVertexAttributes(const glm::vec3& v, const glm::vec3& n, const glm::vec2& t);

                                                   
    // member variables
    int subdivision;
    std::map<std::pair<float, float>, unsigned int> sharedIndices;
    
};

#endif