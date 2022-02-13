#include <Geometry/Sphere.h>


const int MIN_SECTOR_COUNT = 3;
const int MIN_STACK_COUNT = 2;

// TODO: READ THIS 
// https://community.khronos.org/t/generating-tangents-bitangents-for-triangle-strip-sphere/75446

Sphere::Sphere(float radius, int sectors, int stacks) : interleavedStride(56)
{
    set(radius, sectors, stacks);
}

void Sphere::set(float radius, int sectors, int stacks)
{
    this->radius = radius;
    this->sectorCount = sectors;
    if(sectors < MIN_SECTOR_COUNT)
        this->sectorCount = MIN_SECTOR_COUNT;
    this->stackCount = stacks;
    if(stacks < MIN_STACK_COUNT)
        this->stackCount = MIN_STACK_COUNT;

    buildVertices();
}

void Sphere::setRadius(float radius)
{
    if(radius != this->radius)
        set(radius, sectorCount, stackCount);
}

void Sphere::setSectorCount(int sectors)
{
    if(sectors != this->sectorCount)
        set(radius, sectors, stackCount);
}

void Sphere::setStackCount(int stacks)
{
    if(stacks != this->stackCount)
        set(radius, sectorCount, stacks);
}


void Sphere::printSelf() const
{
    std::cout << "====== Sphere ======\n"
              << "         Radius: " << radius << "\n"
              << "   Sector Count:" << sectorCount << "\n"
              << "    Stack Count:" << stackCount << "\n"
              << " Triangle Count:" << getTriangleCount() << "\n"
              << "    Index Count:" << getIndexCount() << "\n"
              << "   Vertex Count:" << getVertexCount() << "\n"
              << "   Normal Count:" << getNormalCount() << "\n"
              << "Bitangent Count:" << getBitangentCount() << "\n"
              << "  Tangent Count:" << getTangentCount() << "\n"
              << " TexCoord Count:" << getTexCoordCount() << std::endl;
}

void Sphere::setupArrayBuffer()
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

void Sphere::setupMesh()
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

// TODO: Will be implemented
void Sphere::draw() const
{
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Sphere::clearArrays()
{
    vertices.clear();
    normals.clear();
    tangents.clear();
    texCoords.clear();
    indices.clear();
}


void Sphere::buildVertices()
{
    const float PI = acos(-1);

    // clear mem
    clearArrays();

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
                addIndices(k1, k2, k1+1);
                std::vector<glm::vec3> tangentBitangents = calcTangentBitangents(vertices[k1], 
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
                addIndices(k1+1, k2, k2+1);

                std::vector<glm::vec3> tangentBitangents = calcTangentBitangents(vertices[k1 + 1], 
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



    setupArrayBuffer();
    setupMesh();
}


void Sphere::addIndices(unsigned int i1, unsigned int i2, unsigned int i3)
{
    indices.push_back(i1);
    indices.push_back(i2);
    indices.push_back(i3);
}

