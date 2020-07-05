
#ifndef _MATRIXTRANSFORM_
#define _MATRIXTRANSFORM_

#include <glm/gtc/matrix_transform.hpp>

class MatrixTransform
{
public:
    MatrixTransform( float d = 1.0f );

    void Translate( const glm::vec3& position );
    void Scale( float xScale, float yScale, float zScale );
    void Rotate( float angleInRad, const glm::vec3& axis );

    inline glm::mat4 AsGlmMat4() const { return _mat4; }

    ~MatrixTransform();

private:

    glm::mat4   _mat4;

};

#endif // !_MATRIXTRANSFORM_
