
#ifndef _MESH_
#define _MESH_

#include <GL/glew.h>

class Mesh
{
public:
    Mesh();

    void Create( GLfloat* vertices, unsigned int nbVertices,
                 unsigned int *indices, unsigned int nbIndices,
                 bool hasNormalData = false,
                 unsigned int dataSetLength = 8u, unsigned int posOffset = 0u,
                 unsigned int textureOffset = 3u, unsigned int normalOffset = 5u );

    void Render();
    void Clear();

    ~Mesh();

private:

    void CalculateNormals( GLfloat* vertices, unsigned int nbVertices,
                           unsigned int *indices, unsigned int nbIndices,
                           unsigned int dataSetLength, unsigned int normalOffset );

    GLuint  _vao,
            _vbo,
            _ibo;

    GLsizei _indexCount;
};

#endif // !_MESH_
