#include <Geometry/RadialSphere.h>


const int MIN_SECTOR_COUNT = 3;
const int MIN_STACK_COUNT = 2;

// TODO: READ THIS 
// https://community.khronos.org/t/generating-tangents-bitangents-for-triangle-strip-sphere/75446

RadialSphere::RadialSphere(float radius, int sectors, int stacks, bool smooth) : Sphere(radius, smooth)
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

    if(smooth)
        BuildVertices();
    else
        BuildVerticesFlat();
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

// Each triangle is independent
void RadialSphere::BuildVerticesFlat()
{
    const float PI = acos(-1);

    // tmp vertex definition
    struct Vertex
    {
        float x,y,z,s,t;
    };

    std::vector<Vertex> tmpVertices;

    float sectorStep  = 2 * PI / sectorCount;
    float stackStep   = PI / stackCount;
    float sectorAngle, stackAngle;

    // compute all vertices first without computing normals
    for(int i=0; i<=stackCount; i++)
    {
        stackAngle = PI / 2 - i * stackStep;      // starting from pi/2 to -pi/2
        float xz   = radius * cosf(stackAngle);   // r * cos(u)
        float y    = radius * sinf(stackAngle);   // r * sin(u)

        // add (sectorCount + 1) vertices per stack
        // the first and last vertices have same position and normal
        // but different tex coords
        for(int j=0; j<=sectorCount; j++)
        {
            sectorAngle = j * sectorStep;         // starting from 0 to 2pi

            Vertex vertex;
            vertex.x = xz * sinf(sectorAngle);    // x = r * cos(u) * sin(v)
            vertex.y = y;                         // y = r * sin(u)
            vertex.z = xz * cosf(sectorAngle);    // z = r * cos(u) * cos(v)
            vertex.s = (float)j / sectorCount;    // s
            vertex.t = (float)i / stackCount;      // t
            tmpVertices.push_back(vertex);
        }
    }

    // clear memory
    ClearArrays();

    Vertex v1, v2, v3, v4;                   // 4 vertex positions and tex coords
    glm::vec3 n;                             // 1 face normal

    int i, j, k, vi1, vi2;
    int index = 0;                           // index for vertex
    for(i=0; i<stackCount; i++)
    {
        vi1 = i * (sectorCount + 1);         // index of tmpVertices
        vi2 = (i + 1) * (sectorCount + 1);

        for(j=0; j<sectorCount; j++, vi1++, vi2++)
        {
            // get 4 vertices per sector
            // v1--v3
            // |    |
            // v2--v4
            v1 = tmpVertices[vi1];
            v2 = tmpVertices[vi2];
            v3 = tmpVertices[vi1 + 1];
            v4 = tmpVertices[vi2 + 1];

            // if 1st stack and last stack, store only 1 triangle per sector
            // otherwise, store 2 triangles (quad) per sector
            if(i==0) // a triangle for first stack -------------------------
            {
                // add vertices of triangle
                vertices.push_back(glm::vec3(v1.x, v1.y, v1.z));
                vertices.push_back(glm::vec3(v2.x, v2.y, v2.z));
                vertices.push_back(glm::vec3(v4.x, v4.y, v4.z));

                // put tex coords of triangle
                texCoords.push_back(glm::vec2(v1.s, v1.t));
                texCoords.push_back(glm::vec2(v2.s, v2.t));
                texCoords.push_back(glm::vec2(v4.s, v4.t));

                // put normal
                n = ComputeFaceNormal(glm::vec3(v1.x,v1.y,v1.z), glm::vec3(v2.x,v2.y,v2.z), glm::vec3(v4.x,v4.y,v4.z));
                for(k=0; k<3; k++) // same normal for three vertices
                {
                    normals.push_back(n);
                }

                // put indices of 1 triangle
                AddIndices(index, index+1, index+2);

                index += 3; // get next index
            }
            else if(i==(stackCount-1)) // a triangle for last stack --------
            {
                // put a triangle
                vertices.push_back(glm::vec3(v1.x, v1.y, v1.z));
                vertices.push_back(glm::vec3(v2.x, v2.y, v2.z));
                vertices.push_back(glm::vec3(v3.x, v3.y, v3.z));

                // put tex coords of triangle
                texCoords.push_back(glm::vec2(v1.s, v1.t));
                texCoords.push_back(glm::vec2(v2.s, v2.t));
                texCoords.push_back(glm::vec2(v3.s, v3.t));

                // put normal
                n = ComputeFaceNormal(glm::vec3(v1.x,v1.y,v1.z), glm::vec3(v2.x,v2.y,v2.z), glm::vec3(v3.x,v3.y,v3.z));               
                for(k=0; k<3; k++) // same normal for three vertices
                {
                    normals.push_back(n);
                }

                // put indices of 1 triangle
                AddIndices(index, index+1, index+2);

                index += 3; // get next index
            }
            else // 2 triangles for others ----------------------------------
            {
                // put quad vertices: v1-v2-v3-v4
                vertices.push_back(glm::vec3(v1.x, v1.y, v1.z));
                vertices.push_back(glm::vec3(v2.x, v2.y, v2.z));
                vertices.push_back(glm::vec3(v3.x, v3.y, v3.z));
                vertices.push_back(glm::vec3(v4.x, v4.y, v4.z));

                // put tex coords of quad
                texCoords.push_back(glm::vec2(v1.s, v1.t));
                texCoords.push_back(glm::vec2(v2.s, v2.t));
                texCoords.push_back(glm::vec2(v3.s, v3.t));
                texCoords.push_back(glm::vec2(v4.s, v4.t));


                // put normal
                n = ComputeFaceNormal(glm::vec3(v1.x,v1.y,v1.z), glm::vec3(v2.x,v2.y,v2.z), glm::vec3(v3.x,v3.y,v3.z));
                for(k=0; k<4; k++) // same normal for four vertices
                {
                    normals.push_back(n);
                }

                // put indices of quad (2 triangles)
                //          v1-v2-v3 and v3-v2-v4
                // v1--v3
                // |    |
                // v2--v4                
                AddIndices(index, index+1, index+2);
                AddIndices(index+2, index+1, index+3);

                index += 4;
            }
        }
    }

    SetupTangentBitangents();
    SetupArrayBuffer();
    SetupMesh();    
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

