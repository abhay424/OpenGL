#include "DirectionalLight.h"

//!-------------------------------------------------------------------
DirectionalLight::DirectionalLight()
    : Light(),
      _direction( 0.0f, -1.0f, 0.0f ) // Deafult downward direction.
{
    _lightProj = glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, 0.1f, 100.0f);
}

//!-------------------------------------------------------------------
glm::vec3 DirectionalLight::Direction(void) const
{
    return _direction;
}

//!-------------------------------------------------------------------
void DirectionalLight::SetDirection( GLfloat xDir, GLfloat yDir, GLfloat zDir )
{
    _direction = glm::vec3(xDir, yDir, zDir);
}

//!-------------------------------------------------------------------
glm::mat4 DirectionalLight::CalculateLightTransform() const
{
    return _lightProj * glm::lookAt(-_direction, glm::vec3(0.0f, 0.0f, 0.0f),
                                                 glm::vec3(0.0f, 1.0f, 0.0f));
}

//!-------------------------------------------------------------------
DirectionalLight::~DirectionalLight()
{
}
