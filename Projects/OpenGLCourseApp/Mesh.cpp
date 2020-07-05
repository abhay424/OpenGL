#include "Mesh.h"

#include <glm/glm.hpp>

//!-------------------------------------------------------------------
Mesh::Mesh() :
    _vao(0),
    _vbo(0),
    _ibo(0),
    _indexCount(0)
{
}

//!-------------------------------------------------------------------
void Mesh::Create( GLfloat* vertices, unsigned int nbVertices,
                   unsigned int* indices, unsigned int nbIndices,
                   bool hasNormalData,
                   unsigned int dataSetLength, unsigned int posOffset,
                   unsigned int textureOffset, unsigned int normalOffset )
{
    _indexCount = nbIndices;

    if( !hasNormalData )
    {
        CalculateNormals( vertices, nbVertices, indices,
                          nbIndices, dataSetLength, normalOffset );
    }

    glGenVertexArrays(1, &_vao);
    glBindVertexArray(_vao);

        glGenBuffers(1, &_ibo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ibo);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * nbIndices, indices, GL_STATIC_DRAW);

            glGenBuffers(1, &_vbo);
            glBindBuffer(GL_ARRAY_BUFFER, _vbo);
                glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0]) * nbVertices, vertices, GL_STATIC_DRAW);

                // For position.
                glVertexAttribPointer(0 /*location*/, 3/*nbInput*/, GL_FLOAT, GL_FALSE,
                                      sizeof(vertices[0]) * dataSetLength /*Single dataSet length*/,
                                      (void*)(sizeof(vertices[0]) * posOffset) /*offset from start of dataSet*/);
                glEnableVertexAttribArray(0 /*location*/);

                // For texture.
                glVertexAttribPointer(1 /*location*/, 2/*nbInput*/, GL_FLOAT, GL_FALSE,
                                      sizeof(vertices[0]) * dataSetLength /*Single dataSet length*/,
                                      (void*)(sizeof(vertices[0]) * textureOffset) /*offset from start of dataSet*/);
                glEnableVertexAttribArray(1 /*location*/);

                // For normal.
                glVertexAttribPointer(2 /*location*/, 3/*nbInput*/, GL_FLOAT, GL_FALSE,
                                      sizeof(vertices[0]) * dataSetLength /*Single dataSet length*/,
                                      (void*)(sizeof(vertices[0]) * normalOffset) /*offset from start of dataSet*/);
                glEnableVertexAttribArray(2 /*location*/);

            glBindBuffer(GL_ARRAY_BUFFER, 0);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glBindVertexArray(0);
}

//!-------------------------------------------------------------------
void Mesh::CalculateNormals( GLfloat* vertices, unsigned int nbVertices,
                             unsigned int* indices, unsigned int nbIndices,
                             unsigned int dataSetLength, unsigned int normalOffset )
{
    for( unsigned int i = 0; i < nbIndices; i += 3 )
    {
        // Get staring indices of three vertices of triangle.
        unsigned int index0 = indices[i]     * dataSetLength;
        unsigned int index1 = indices[i + 1] * dataSetLength;
        unsigned int index2 = indices[i + 2] * dataSetLength;

        // Get two sides of the triangle.
        glm::vec3 v01( vertices[index1]     - vertices[index0],
                       vertices[index1 + 1] - vertices[index0 + 1],
                       vertices[index1 + 2] - vertices[index0 + 2] );
        glm::vec3 v02( vertices[index2]     - vertices[index0],
                       vertices[index2 + 1] - vertices[index0 + 1],
                       vertices[index2 + 2] - vertices[index0 + 2] );

        // Using cross product, calculate normal of the triangular surface.
        glm::vec3 normal = glm::cross(v01, v02);
        normal = glm::normalize(normal);

        // Add normal to existing value. It will be later normalized.
        // Get starting index of the normal for vertices of the triangle.
        index0 += normalOffset;
        index1 += normalOffset;
        index2 += normalOffset;

        // Will add calculated normal to the normals of all the 3 vertices of this triangle.
        vertices[index0] += normal.x; vertices[index0 + 1] += normal.y; vertices[index0 + 2] += normal.z;
        vertices[index1] += normal.x; vertices[index1 + 1] += normal.y; vertices[index1 + 2] += normal.z;
        vertices[index2] += normal.x; vertices[index2 + 1] += normal.y; vertices[index2 + 2] += normal.z;
    }

    // Now normalize each of the normals.
    for( unsigned int i = 0; i < nbVertices/dataSetLength; ++i )
    {
        unsigned int normalIndex = (i * dataSetLength) + normalOffset;
        glm::vec3 normal(vertices[normalIndex], vertices[normalIndex + 1], vertices[normalIndex + 2]);
        normal = glm::normalize(normal);
        vertices[normalIndex]     = normal.x;
        vertices[normalIndex + 1] = normal.y;
        vertices[normalIndex + 2] = normal.z;
    }
}

//!-------------------------------------------------------------------
void Mesh::Render()
{
    glBindVertexArray(_vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ibo);
        glDrawElements(GL_TRIANGLES, _indexCount, GL_UNSIGNED_INT, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

//!-------------------------------------------------------------------
void Mesh::Clear()
{
    if( _ibo != 0 )
    {
        glDeleteBuffers(1, &_ibo);
        _ibo = 0;
    }

    if( _vbo != 0 )
    {
        glDeleteBuffers(1, &_vbo);
        _vbo = 0;
    }

    if( _vao != 0 )
    {
        glDeleteVertexArrays(1, &_ibo);
        _ibo = 0;
    }
    _indexCount = 0;
}

//!-------------------------------------------------------------------
Mesh::~Mesh()
{
    Clear();
}
