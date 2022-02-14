#include <Geometry/RadialSphere.h>


const int MIN_SECTOR_COUNT = 3;
const int MIN_STACK_COUNT = 2;

// TODO: READ THIS 
// https://community.khronos.org/t/generating-tangents-bitangents-for-triangle-strip-sphere/75446

RadialSphere::RadialSphere(float radius, int sectors, int stacks) : Sphere(radius)
{
    Set(radius, sectors, stacks);
}

void RadialSphere::Set(float radius, int sectors, int stacks)
{
    this->radius = radius;
    this->sectorCount = sectors;
    if(sectors < MIN_SECTOR_COUNT)
        this->sectorCount = MIN_SECTOR_COUNT;
    this->stackCount = stacks;
    if(stacks < MIN_STACK_COUNT)
        this->stackCount = MIN_STACK_COUNT;

    BuildVertices();
}

void RadialSphere::SetRadius(float radius)
{
    if(radius != this->radius)
        Set(radius, sectorCount, stackCount);
}

void RadialSphere::SetSectorCount(int sectors)
{
    if(sectors != this->sectorCount)
        Set(radius, sectors, stackCount);
}

void RadialSphere::SetStackCount(int stacks)
{
    if(stacks != this->stackCount)
        Set(radius, sectorCount, stacks);
}


void RadialSphere::PrintSelf() const
{
    std::cout << "====== Radial Sphere ======\n"
              << "         Radius: " << radius << "\n"
              << "   Sector Count:" << sectorCount << "\n"
              << "    Stack Count:" << stackCount << "\n"
              << " Triangle Count:" << GetTriangleCount() << "\n"
              << "    Index Count:" << GetIndexCount() << "\n"
              << "   Vertex Count:" << GetVertexCount() << "\n"
              << "   Normal Count:" << GetNormalCount() << "\n"
              << "Bitangent Count:" << GetBitangentCount() << "\n"
              << "  Tangent Count:" << GetTangentCount() << "\n"
              << " TexCoord Count:" << GetTexCoordCount() << std::endl;
}



void RadialSphere::BuildVertices()
{
    const float PI = acos(-1);

    // clear mem
    ClearArrays();

    float xz;                            // position of vertex

    float sectorStep = 2 * PI / sectorCount;
    float stackStep  = PI / stackCount;
    float sectorAngle, stackAngle;

    glm::vec3 vertex(0.0f, 0.0f, 0.0f);
    glm::vec2 texCoord(0.0f, 0.0f);

    for(int i=0; i<=stackCount; i++)
    {
        // stack angle is fi
        stackAngle = PI / 2 - i * stackStep; // starting from pi/2 to -pi/2
        xz = radius * cosf(stackAngle);
        vertex.y  = radius * sinf(stackAngle);

        // add (sectorCount+1) vertices per stack
        // the first and last vertices have same position and normal, but different tex coords
        for(int j=0; j<=sectorCount; j++)
        {
            sectorAngle = j * sectorStep;

            // vertex position
            vertex.z = xz * cosf(sectorAngle);
            vertex.x = xz * sinf(sectorAngle);
            vertices.push_back(vertex);

            // normalized vertex normal
            vertex = glm::normalize(vertex);
            normals.push_back(vertex);

            // vertex tex coord between [0, 1]
            texCoord.x = (float)j / sectorCount;
            texCoord.y = (float)i / stackCount;
            texCoords.push_back(texCoord);
        }
    }

    // initialize tangent and bitangent
    tangents   = std::vector<glm::vec3>(normals.size(), glm::vec3(0.0f));
    bitangents = std::vector<glm::vec3>(normals.size(), glm::vec3(0.0f));

    // indices
    //  k1--k1+1
    //  |  / |
    //  | /  |
    //  k2--k2+1
    unsigned int k1, k2;
    for(int i=0; i<stackCount; i++)
    {
        k1 = i * (sectorCount + 1); // beginning of current stack
        k2 = k1 + sectorCount + 1;  // beginning of next stack

        for(int j=0; j<sectorCount; j++, k1++, k2++)
        {
            // 2 triangles per sector excluding 1st and last stacks
            if(i != 0)
            {
                AddIndices(k1, k2, k1+1);
                std::vector<glm::vec3> tangentBitangents = CalcTangentBitangents(vertices[k1], 
                                                                                 vertices[k2],
                                                                                 vertices[k1+1],
                                                                                 texCoords[k1],
                                                                                 texCoords[k2],
                                                                                 texCoords[k1+1]);
                tangents[k1]   += tangentBitangents[0];
                bitangents[k1] += tangentBitangents[1];

                tangents[k2]   += tangentBitangents[0];
                bitangents[k2] += tangentBitangents[1];

                tangents[k1 + 1]   += tangentBitangents[0];
                bitangents[k1 + 1] += tangentBitangents[1];                
            }
            if(i != (stackCount-1))
            {
                AddIndices(k1+1, k2, k2+1);

                std::vector<glm::vec3> tangentBitangents = CalcTangentBitangents(vertices[k1 + 1], 
                                                                                 vertices[k2],
                                                                                 vertices[k2 + 1],
                                                                                 texCoords[k1 + 1],
                                                                                 texCoords[k2],
                                                                                 texCoords[k2+1]);                
                tangents[k1 + 1]   += tangentBitangents[0];
                bitangents[k1 + 1] += tangentBitangents[1];

                tangents[k2]   += tangentBitangents[0];
                bitangents[k2] += tangentBitangents[1];

                tangents[k2 + 1]   += tangentBitangents[0];
                bitangents[k2 + 1] += tangentBitangents[1];                  
            }
        }
    }

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
        if(glm::dot(glm::cross(tangent,normal), bitangent) < 0.0f)
            tangent *= -1.0f;

        bitangent = glm::normalize(glm::cross(tangent, normal));
    }



    SetupArrayBuffer();
    SetupMesh();
}

