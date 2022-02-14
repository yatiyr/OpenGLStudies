#include <Geometry/Sphere.h>


Sphere::Sphere(float radius, bool smooth) : radius(radius), smooth(smooth) {}

Sphere::~Sphere()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

void Sphere::SetSmooth(bool smooth)
{
    this->smooth = smooth;

    if(smooth)
        BuildVertices();
    else
        BuildVerticesFlat();
}

unsigned int Sphere::GetVertexCount() const     {return (unsigned int) vertices.size() / 3;}
unsigned int Sphere::GetNormalCount() const     {return (unsigned int) normals.size() / 3;}
unsigned int Sphere::GetBitangentCount() const  {return (unsigned int) bitangents.size() / 3;}
unsigned int Sphere::GetTangentCount() const    {return (unsigned int) tangents.size() / 3;}
unsigned int Sphere::GetTexCoordCount() const   {return (unsigned int) texCoords.size() / 2;}
unsigned int Sphere::GetIndexCount() const      {return (unsigned int)indices.size();}
unsigned int Sphere::GetTriangleCount() const   {return GetIndexCount() / 3;}
unsigned int Sphere::GetVertexSize() const      {return (unsigned int)vertices.size() * sizeof(float);}
unsigned int Sphere::GetNormalSize() const      {return (unsigned int)normals.size() * sizeof(float);}
unsigned int Sphere::GetTangentSize() const     {return (unsigned int)tangents.size() * sizeof(float);}
unsigned int Sphere::GetBitangentSize() const   {return (unsigned int)bitangents.size() * sizeof(float);}
unsigned int Sphere::GetTexCoordSize() const    {return (unsigned int)texCoords.size() * sizeof(float);}
unsigned int Sphere::GetIndexSize() const       {return (unsigned int)indices.size() * sizeof(unsigned int);}
float        Sphere::GetRadius() const          {return radius;}

const std::vector<glm::vec3> Sphere::GetVertices()   const {return vertices;}
const std::vector<glm::vec3> Sphere::GetNormals()    const {return normals;}
const std::vector<glm::vec3> Sphere::GetTangents()   const {return tangents;}
const std::vector<glm::vec3> Sphere::GetBitangents() const {return bitangents;}
const std::vector<glm::vec2> Sphere::GetTexCoords()  const {return texCoords;}
const unsigned int* Sphere::GetIndices() const {return indices.data();}
// This function gives tangent and bitangent vectors from a triangle
// providing 3 position and 3 texture coordinates
std::vector<glm::vec3> Sphere::CalcTangentBitangents(glm::vec3 pos1, glm::vec3 pos2, glm::vec3 pos3,
                                                     glm::vec2 uv1, glm::vec2 uv2, glm::vec2 uv3)
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


void Sphere::SetupArrayBuffer()
{
    arrayBuffer.clear();

    for(unsigned int i=0; i<vertices.size(); i++)
    {
        // Add position
        arrayBuffer.push_back(vertices[i].x);
        arrayBuffer.push_back(vertices[i].y);
        arrayBuffer.push_back(vertices[i].z);

        // Add normal
        arrayBuffer.push_back(normals[i].x);
        arrayBuffer.push_back(normals[i].y);
        arrayBuffer.push_back(normals[i].z);

        // Add texture coords
        arrayBuffer.push_back(texCoords[i].x);
        arrayBuffer.push_back(texCoords[i].y);

        // Add tangents
        arrayBuffer.push_back(tangents[i].x);
        arrayBuffer.push_back(tangents[i].y);
        arrayBuffer.push_back(tangents[i].z);

        // Add bitangents
        arrayBuffer.push_back(bitangents[i].x);
        arrayBuffer.push_back(bitangents[i].y);
        arrayBuffer.push_back(bitangents[i].z);        
    }
}

void Sphere::SetupMesh()
{
    // clear buffers if they are already allocated
    if(VAO != 0)
    {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
    }

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, arrayBuffer.size() * sizeof(float), &arrayBuffer[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    // position data of vertices
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 56, (void*)0);

    // normal data of vertices
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 56, (void*)(3 * sizeof(float)));

    // texture coordinates of vertices
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 56, (void*)(6 * sizeof(float)));

    // tangents of vertices
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 56, (void*)(8 * sizeof(float)));

    // bitangents of vertices
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 56, (void *)(11 * sizeof(float)));

    // unbind vertex array
    glBindVertexArray(0);
}

void Sphere::Draw() const
{
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Sphere::ClearArrays()
{
    vertices.clear();
    normals.clear();
    tangents.clear();
    bitangents.clear();
    texCoords.clear();
    indices.clear();
}

void Sphere::AddIndices(unsigned int i1, unsigned int i2, unsigned int i3)
{
    indices.push_back(i1);
    indices.push_back(i2);
    indices.push_back(i3);    
}


void Sphere::SetupTangentBitangents()
{

    // initialize tangent and bitangent
    tangents   = std::vector<glm::vec3>(normals.size(), glm::vec3(0.0f));
    bitangents = std::vector<glm::vec3>(normals.size(), glm::vec3(0.0f));

    // initialize vectors for vertices and texture coords
    glm::vec3 v1, v2, v3;
    glm::vec2 t1, t2, t3;

    // traverse all triangles and calculate tangents and bitangents
    // for all vertices
    for(int i=0; i<indices.size(); i+=3)
    {
        v1 = vertices[indices[i]];
        v2 = vertices[indices[i+1]];
        v3 = vertices[indices[i+2]];

        t1 = texCoords[indices[i]];
        t2 = texCoords[indices[i+1]];
        t3 = texCoords[indices[i+2]];

        // Get Tangent and Bitangent Vectors
        std::vector<glm::vec3> TB = CalcTangentBitangents(v1,v2,v3,
                                                          t1,t2,t3);

        tangents[indices[i]]     += TB[0];
        bitangents[indices[i]]   += TB[1];

        tangents[indices[i+1]]   += TB[0];
        bitangents[indices[i+1]] += TB[1];

        tangents[indices[i+2]]   += TB[0];
        bitangents[indices[i+2]] += TB[1];        


    }

    // orthogonalize and normalize tangents and bitangents   
    for(int i=0; i<vertices.size(); i++)
    {
        glm::vec3 &normal = normals[i];
        glm::vec3 &tangent = tangents[i];
        glm::vec3 &bitangent = bitangents[i];

        normal = glm::normalize(normal);
        tangent = glm::normalize(tangent);
        bitangent = glm::normalize(bitangent);

        // gram-schmidt orthogonalize
        tangent = glm::normalize(tangent - normal * glm::dot(normal, tangent));

        // calculate handedness
        if(glm::dot(glm::cross(tangent, normal), bitangent) < 0.0f)
            tangent *= -1.0f;

        bitangent = glm::normalize(glm::cross(tangent, normal));
    }    
}


glm::vec3 Sphere::ComputeFaceNormal(const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3)
{
    const float EPSILON = 0.000001f;

    glm::vec3 normal(0.0f);
    
    glm::vec3 e1 = v2 - v1;
    glm::vec3 e2 = v3 - v1;

    normal = glm::cross(e1, e2);
    if(glm::length(normal) > EPSILON)
    {
        normal = glm::normalize(normal);
        return normal;
    }
    

    return glm::vec3(0.0f);
}
