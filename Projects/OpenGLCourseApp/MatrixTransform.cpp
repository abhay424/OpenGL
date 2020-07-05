
#include "MatrixTransform.h"

//!-------------------------------------------------------------------
MatrixTransform::MatrixTransform( float d )
    : _mat4(d)
{
}

//!-------------------------------------------------------------------
void MatrixTransform::Translate( const glm::vec3& position )
{
    _mat4 = glm::translate(_mat4, position);
}

//!-------------------------------------------------------------------
void MatrixTransform::Scale( float xScale, float yScale, float zScale )
{
    _mat4 = glm::scale(_mat4, glm::vec3(xScale, yScale, zScale));
}

//!-------------------------------------------------------------------
void MatrixTransform::Rotate( float angleInRad, const glm::vec3& axis )
{
    _mat4 = glm::rotate(_mat4, angleInRad, axis);
}

//!-------------------------------------------------------------------
MatrixTransform::~MatrixTransform()
{
}
