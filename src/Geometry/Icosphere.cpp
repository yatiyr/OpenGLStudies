#include <Geometry/Icosphere.h>


Icosphere::Icosphere(float radius, int subdivision) : Sphere(radius), subdivision(subdivision)
{
    BuildVertices();
}


void Icosphere::SetRadius(float radius)
{
    if(radius > 0)
    {
        this->radius = radius;
        UpdateRadius();
    }
}

void Icosphere::UpdateRadius()
{
    float scale = ComputeScaleForLength(vertices[0], radius);
    std::size_t i, j;
    std::size_t count = vertices.size();

    for(i=0, j=0; i<count; i+=3, j+=14)
    {
        vertices[i] *= scale;
        
        arrayBuffer[j]   *= scale;
        arrayBuffer[j+1] *= scale;
        arrayBuffer[j+2] *= scale;
    }
}

void Icosphere::SetSubdivision(int subdivision)
{
    this->subdivision = subdivision;
    BuildVertices();
}

void Icosphere::PrintSelf() const
{
    std::cout << "====== Sphere ======\n"
              << "         Radius: " << radius << "\n"
              << "Subdivision Count:" << subdivision << "\n"
              << "   Triangle Count:" << GetTriangleCount() << "\n"
              << "      Index Count:" << GetIndexCount() << "\n"
              << "     Vertex Count:" << GetVertexCount() << "\n"
              << "     Normal Count:" << GetNormalCount() << "\n"
              << "  Bitangent Count:" << GetBitangentCount() << "\n"
              << "    Tangent Count:" << GetTangentCount() << "\n"
              << "   TexCoord Count:" << GetTexCoordCount() << std::endl;
}

// Instead of the code I saw at "http://www.songho.ca/opengl/gl_sphere.html"
// I wanted to use a right hand coordinate system where bottom to top vector
// is aligned with y axis
std::vector<glm::vec3> Icosphere::ComputeIcosahedronVertices()
{
    const float PI = acos(-1);
    const float H_ANGLE = PI / 180 * 72;    // 72 degrees in radians
    const float V_ANGLE = atanf(1.0f / 2);  // elevation 26.565 degree

    std::vector<glm::vec3> verts(12);    // 12 vertices
    int i1, i2;                             // indices
    float y, xz;                            // coords
    float hAngle1 = PI / 2 + H_ANGLE / 2;  // start from  126 deg at 2nd row
    float hAngle2 = PI / 2;                // start from   90 deg at 3rd row

    // the first top vertex (0, r, 0)
    verts[0].x = 0;
    verts[0].y = radius;
    verts[0].z = 0;

    // 10 vertices at 2nd and 3rd rows
    for(int i=1; i<=5; i++)
    {
        i1 = i;        // for 2nd row
        i2 = (i + 5);  // for 3rd row

        y  = radius * sinf(V_ANGLE);
        xz = radius * cosf(V_ANGLE);

        verts[i1].x = xz * cosf(hAngle1);
        verts[i2].x = xz * cosf(hAngle2);
        verts[i1].y =  y;
        verts[i2].y = -y;
        verts[i1].z = xz * sinf(hAngle1);
        verts[i2].z = xz * sinf(hAngle2);

        // next horizontal angles
        hAngle1 -= H_ANGLE;
        hAngle2 -= H_ANGLE;
    }

    // the last bottom vertex (0, -r, 0)
    verts[11].x = 0;
    verts[11].y = -radius;
    verts[11].z = 0;

    return verts;

}

void Icosphere::BuildVertices()
{
    const float S_STEP = 1 / 11.0f;
    const float T_STEP = 1 / 3.0f;

    // compute 12 vertices of icosahedron
    // NOTE: v0 (top), v11 (bottom), v1, v6 (first vertices on each row) cannot
    // be shared for smooth shading (they have different texcoords)
    std::vector<glm::vec3> tmpVertices = ComputeIcosahedronVertices();


    // clear memory
    ClearArrays();

    glm::vec3 vertex;
    glm::vec3 normal;
    float scale;

    // smooth icosahedron has 14 non-shared (0 to 13) and
    // 8 shared vertices (14 to 21) (total 22 vertices)
    //  00  01  02  03  04          //
    //  /\  /\  /\  /\  /\          //
    // /  \/  \/  \/  \/  \         //
    //10--14--15--16--17--11        //
    // \  /\  /\  /\  /\  /\        //
    //  \/  \/  \/  \/  \/  \       //
    //  12--18--19--20--21--13      //
    //   \  /\  /\  /\  /\  /       //
    //    \/  \/  \/  \/  \/        //
    //    05  06  07  08  09        //
    // add 14 non-shared vertices first (index from 0 to 13)
    vertices.push_back(tmpVertices[0]);                        // v0 (top)
    normals.push_back(tmpVertices[0]);
    texCoords.push_back(glm::vec2(S_STEP, 0));

    vertices.push_back(tmpVertices[0]);                        // v1
    normals.push_back(tmpVertices[0]);
    texCoords.push_back(glm::vec2(S_STEP * 3, 0));

    vertices.push_back(tmpVertices[0]);                        // v2
    normals.push_back(tmpVertices[0]);
    texCoords.push_back(glm::vec2(S_STEP * 5, 0));

    vertices.push_back(tmpVertices[0]);                        // v3
    normals.push_back(tmpVertices[0]);
    texCoords.push_back(glm::vec2(S_STEP * 7, 0));

    vertices.push_back(tmpVertices[0]);                        // v4
    normals.push_back(tmpVertices[0]);
    texCoords.push_back(glm::vec2(S_STEP * 9, 0));

    vertices.push_back(tmpVertices[11]);                       // v5 (bottom)
    normals.push_back(tmpVertices[11]);
    texCoords.push_back(glm::vec2(S_STEP * 2, T_STEP * 3));

    vertices.push_back(tmpVertices[11]);                       // v6
    normals.push_back(tmpVertices[11]);
    texCoords.push_back(glm::vec2(S_STEP * 4, T_STEP * 3));

    vertices.push_back(tmpVertices[11]);                       // v7
    normals.push_back(tmpVertices[11]);
    texCoords.push_back(glm::vec2(S_STEP * 6, T_STEP * 3));

    vertices.push_back(tmpVertices[11]);                       // v8
    normals.push_back(tmpVertices[11]);
    texCoords.push_back(glm::vec2(S_STEP * 8, T_STEP * 3));

    vertices.push_back(tmpVertices[11]);                       // v9
    normals.push_back(tmpVertices[11]);
    texCoords.push_back(glm::vec2(S_STEP * 10, T_STEP * 3));

    normal.x = tmpVertices[1].x;                               // v10 (left)
    normal.y = tmpVertices[1].y;
    normal.z = tmpVertices[1].z;
    normal = glm::normalize(normal);
    vertices.push_back(tmpVertices[1]);
    normals.push_back(glm::vec3(1.0,1.0,1.0));
    texCoords.push_back(glm::vec2(0, T_STEP));

    vertices.push_back(tmpVertices[1]);                        // v11 (right)
    normals.push_back(normal);
    texCoords.push_back(glm::vec2(S_STEP * 10, T_STEP));

    normal.x = tmpVertices[6].x;                               // v12 (left)
    normal.y = tmpVertices[6].y;
    normal.z = tmpVertices[6].z;
    normal = glm::normalize(normal);
    vertices.push_back(tmpVertices[6]);
    normals.push_back(normal);
    texCoords.push_back(glm::vec2(S_STEP, T_STEP * 2));

    vertices.push_back(tmpVertices[6]);                        // v13 (right)
    normals.push_back(normal);
    texCoords.push_back(glm::vec2(S_STEP * 11, T_STEP * 2));

    // add 8 shared vertices to array (index from 14 to 21)
    normal.x = tmpVertices[2].x;                               // v14 (shared)
    normal.y = tmpVertices[2].y;
    normal.z = tmpVertices[2].z;
    normal = glm::normalize(normal);
    vertices.push_back(tmpVertices[2]);
    normals.push_back(normal);
    texCoords.push_back(glm::vec2(S_STEP * 2, T_STEP));
    sharedIndices[std::make_pair(S_STEP * 2, T_STEP)] = texCoords.size() / 2 - 1;
    
    normal.x = tmpVertices[3].x;                               // v15 (shared)
    normal.y = tmpVertices[3].y;
    normal.z = tmpVertices[3].z;
    normal = glm::normalize(normal);
    vertices.push_back(tmpVertices[3]);
    normals.push_back(normal);
    texCoords.push_back(glm::vec2(S_STEP * 4, T_STEP));
    sharedIndices[std::make_pair(S_STEP * 4, T_STEP)] = texCoords.size() / 2 - 1;

    normal.x = tmpVertices[4].x;                               // v16 (shared)
    normal.y = tmpVertices[4].y;
    normal.z = tmpVertices[4].z;
    normal = glm::normalize(normal);
    vertices.push_back(tmpVertices[4]);
    normals.push_back(normal);
    texCoords.push_back(glm::vec2(S_STEP * 6, T_STEP));
    sharedIndices[std::make_pair(S_STEP * 6, T_STEP)] = texCoords.size() / 2 - 1;

    normal.x = tmpVertices[5].x;                               // v17 (shared)
    normal.y = tmpVertices[5].y;
    normal.z = tmpVertices[5].z;
    normal = glm::normalize(normal);
    vertices.push_back(tmpVertices[5]);
    normals.push_back(normal);
    texCoords.push_back(glm::vec2(S_STEP * 8, T_STEP));
    sharedIndices[std::make_pair(S_STEP * 8, T_STEP)] = texCoords.size() / 2 - 1;

    normal.x = tmpVertices[7].x;                               // v18 (shared)
    normal.y = tmpVertices[7].y;
    normal.z = tmpVertices[7].z;
    normal = glm::normalize(normal);
    vertices.push_back(tmpVertices[7]);
    normals.push_back(normal);
    texCoords.push_back(glm::vec2(S_STEP * 3, T_STEP * 2));
    sharedIndices[std::make_pair(S_STEP * 3, T_STEP * 2)] = texCoords.size() / 2 - 1;

    normal.x = tmpVertices[8].x;                               // v19 (shared)
    normal.y = tmpVertices[8].y;
    normal.z = tmpVertices[8].z;
    normal = glm::normalize(normal);
    vertices.push_back(tmpVertices[8]);
    normals.push_back(normal);
    texCoords.push_back(glm::vec2(S_STEP * 5, T_STEP * 2));
    sharedIndices[std::make_pair(S_STEP * 5, T_STEP * 2)] = texCoords.size() / 2 - 1;

    normal.x = tmpVertices[9].x;                               // v20 (shared)
    normal.y = tmpVertices[9].y;
    normal.z = tmpVertices[9].z;
    normal = glm::normalize(normal);
    vertices.push_back(tmpVertices[9]);
    normals.push_back(normal);
    texCoords.push_back(glm::vec2(S_STEP * 7, T_STEP * 2));
    sharedIndices[std::make_pair(S_STEP * 7, T_STEP * 2)] = texCoords.size() / 2 - 1;

    normal.x = tmpVertices[10].x;                               // v21 (shared)
    normal.y = tmpVertices[10].y;
    normal.z = tmpVertices[10].z;
    normal = glm::normalize(normal);
    vertices.push_back(tmpVertices[10]);
    normals.push_back(normal);
    texCoords.push_back(glm::vec2(S_STEP * 9, T_STEP * 2));
    sharedIndices[std::make_pair(S_STEP * 9, T_STEP * 2)] = texCoords.size() / 2 - 1;   


    // build index list for icosahedron (20 triangles)
    AddIndices( 0, 10, 14);      // 1st row (5 tris)     
    AddIndices( 1, 14, 15);
    AddIndices( 2, 15, 16);
    AddIndices( 3, 16, 17);
    AddIndices( 4, 17, 11);
    AddIndices(10, 12, 14);      // 2nd row (10 tris)
    AddIndices(12, 18, 14);
    AddIndices(14, 18, 15);
    AddIndices(18, 19, 15);
    AddIndices(15, 19, 16);
    AddIndices(19, 20, 16);
    AddIndices(16, 20, 17);
    AddIndices(20, 21, 17);
    AddIndices(17, 21, 11);
    AddIndices(21, 13, 11);
    AddIndices( 5, 18, 12);      // 3rd row (5 tris)
    AddIndices( 6, 19, 18);
    AddIndices( 7, 20, 19);
    AddIndices( 8, 21, 20);
    AddIndices( 9, 13, 21);

    // subdivide icosahedron
    SubdivideVertices();


    SetupTangentBitangents();
    SetupArrayBuffer();
    SetupMesh();
}


///////////////////////////////////////////////////////////////////////////////
// divide a trianlge (v1-v2-v3) into 4 sub triangles by adding middle vertices
// (newV1, newV2, newV3) and repeat N times
// If subdivision=0, do nothing.
//         v1           //
//        / \           //
// newV1 *---* newV3    //
//      / \ / \         //
//    v2---*---v3       //
//        newV2         //
///////////////////////////////////////////////////////////////////////////////
void Icosphere::SubdivideVertices()
{
    std::vector<unsigned int> tmpIndices;
    int indexCount;
    unsigned int i1, i2, i3;            // indices from original triangle
    glm::vec3 v1, v2, v3;               // original vertices of a triangle
    glm::vec2 t1, t2, t3;               // original texcoords of a triangle

    glm::vec3 newV1, newV2, newV3;      // new subdivided vertex positions
    glm::vec3 newN1, newN2, newN3;      // new subdivided normals
    glm::vec2 newT1, newT2, newT3;      // new subdivided texture coords
    unsigned int newI1, newI2, newI3;   // new subdivided indices
    int i, j;

    // iteration for subdivision
    for(i=1; i<=subdivision; i++)
    {
        // copy prev indices
        tmpIndices = indices;

        // clear prev indices
        // because we will add new ones
        indices.clear();

        indexCount = (int)tmpIndices.size();
        for(j = 0; j<indexCount; j+=3)
        {
            // get 3 indices of each triangle
            i1 = tmpIndices[j];
            i2 = tmpIndices[j + 1];
            i3 = tmpIndices[j + 2];

            // get 3 vertex attribs from prev triangle
            v1 = vertices[i1];
            v2 = vertices[i2];
            v3 = vertices[i3];

            t1 = texCoords[i1];
            t2 = texCoords[i2];
            t3 = texCoords[i3];


            // get 3 new vertex attribs by splitting half on each edge
            ComputeHalfVertex(v1, v2, radius, newV1);
            ComputeHalfVertex(v2, v3, radius, newV2);
            ComputeHalfVertex(v1, v3, radius, newV3);

            ComputeHalfTexCoord(t1, t2, newT1);
            ComputeHalfTexCoord(t2, t3, newT2);
            ComputeHalfTexCoord(t1, t3, newT3);

            ComputeVertexNormal(newV1, newN1);
            ComputeVertexNormal(newV2, newN2);
            ComputeVertexNormal(newV3, newN3);

            // add new vertices/normals/texcoords to arrays
            // it will check if it is shared/non-shared and
            // return index
            newI1 = AddSubVertexAttributes(newV1, newN1, newT1);
            newI2 = AddSubVertexAttributes(newV2, newN2, newT2);
            newI3 = AddSubVertexAttributes(newV3, newN3, newT3);

            // add 4 new triangle indices
            AddIndices(i1, newI1, newI3);
            AddIndices(newI1, i2, newI2);
            AddIndices(newI1, newI2, newI3);
            AddIndices(newI3, newI2, i3);
        }
    }

}

float Icosphere::ComputeScaleForLength(glm::vec3 vertex, float length)
{
    return length / glm::length(vertex);
}

void Icosphere::ComputeHalfVertex(const glm::vec3& v1, const glm::vec3& v2, float length, glm::vec3& newVertex)
{
    newVertex = v1 + v2;
    float scale = ComputeScaleForLength(newVertex, length);
    newVertex *= scale;
}

void Icosphere::ComputeHalfTexCoord(const glm::vec2& t1, const glm::vec2& t2, glm::vec2& newTexCoord)
{
    newTexCoord = (t1 + t2) * 0.5f;
}

unsigned int Icosphere::AddSubVertexAttributes(const glm::vec3& v, const glm::vec3& n, const glm::vec2& t)
{
    unsigned int index;

    // check if vertex is shared or not
    if(Icosphere::IsSharedTexCoord(t))
    {
        // find if it does already exist in sharedIndices map using (s,t) key
        // if not in the list, add the vertex attribs to arrays and return its index
        // if exists, return the current index
        std::pair<float, float> key = std::make_pair(t.x, t.y);
        std::map<std::pair<float, float>, unsigned int>::iterator iter = sharedIndices.find(key);
        if(iter == sharedIndices.end())
        {
            vertices.push_back(v);
            normals.push_back(n);
            texCoords.push_back(t);
            index = texCoords.size() - 1;
            sharedIndices[key] = index;
        }
        else
        {           
            index = iter->second;
        }
    }
    else
    {     
        vertices.push_back(v);;
        normals.push_back(n);
        texCoords.push_back(t);
        index = texCoords.size() - 1;
    }
    return index;
}

bool Icosphere::IsOnLineSegment(const glm::vec2& a, const glm::vec2& b, const glm::vec2& c)
{
    const float EPSILON = 0.0001f;

    // cross product must be 0 if c is on the line
    glm::vec2 ab = b - a;
    glm::vec2 ac = c - a;
    float cross = ab.x * ac.y - ab.y * ac.x;

    if(cross > EPSILON || cross < - EPSILON)
        return false;


    // c must be within a-b
    if((c.x > a.x && c.x > b.x) || (c.x < a.x && c.x < b.x))
        return false;
    if((c.y > a.y && c.y > b.y) || (c.y < a.y && c.y < b.y))
        return false;

    return true;
}

///////////////////////////////////////////////////////////////////////////////
// This function used 20 non-shared line segments to determine if the given
// texture coordinate is shared or no. If it is on the line segments, it is also
// non-shared point
//   00  01  02  03  04         //
//   /\  /\  /\  /\  /\         //
//  /  \/  \/  \/  \/  \        //
// 05  06  07  08  09   \       //
//   \   10  11  12  13  14     //
//    \  /\  /\  /\  /\  /      //
//     \/  \/  \/  \/  \/       //
//      15  16  17  18  19      //
///////////////////////////////////////////////////////////////////////////////
bool Icosphere::IsSharedTexCoord(const glm::vec2& t)
{
    const float S = 1.0f / 11;
    const float T = 1.0f / 3;

    static float segments[] = { S, 0,       0, T,       // 00 - 05
                                S, 0,       S*2, T,     // 00 - 06
                                S*3, 0,     S*2, T,     // 01 - 06
                                S*3, 0,     S*4, T,     // 01 - 07
                                S*5, 0,     S*4, T,     // 02 - 07
                                S*5, 0,     S*6, T,     // 02 - 08
                                S*7, 0,     S*6, T,     // 03 - 08
                                S*7, 0,     S*8, T,     // 03 - 09
                                S*9, 0,     S*8, T,     // 04 - 09
                                S*9, 0,     1, T*2,     // 04 - 14
                                0, T,       S*2, 1,     // 05 - 15
                                S*3, T*2,   S*2, 1,     // 10 - 15
                                S*3, T*2,   S*4, 1,     // 10 - 16
                                S*5, T*2,   S*4, 1,     // 11 - 16
                                S*5, T*2,   S*6, 1,     // 11 - 17
                                S*7, T*2,   S*6, 1,     // 12 - 17
                                S*7, T*2,   S*8, 1,     // 12 - 18
                                S*9, T*2,   S*8, 1,     // 13 - 18
                                S*9, T*2,   S*10, 1,    // 13 - 19
                                1, T*2,     S*10, 1 };  // 14 - 19

    int count = (int)(sizeof(segments) / sizeof(segments[0]));
    for(int i=0, j=2; i<count; i+=4, j+=4)
    {
        glm::vec2 a(segments[i], segments[i+1]);
        glm::vec2 b(segments[j], segments[j+1]);
        if(IsOnLineSegment(a, b, t))
        {

            return false;
        }
    }

    return true;                              
}

void Icosphere::ComputeVertexNormal(const glm::vec3& vertex, glm::vec3& normal)
{
    normal = glm::normalize(vertex);
}