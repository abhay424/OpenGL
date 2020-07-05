
#include "SpotLight.h"

//!-------------------------------------------------------------------
SpotLight::SpotLight()
    : PointLight(),
      _direction(0.0f, -1.0f, 0.0f),
      _edge(0.0f)
{
}

//!-------------------------------------------------------------------
void SpotLight::SetDirection( GLfloat xDir, GLfloat yDir, GLfloat zDir )
{
    _direction = glm::normalize(glm::vec3(xDir, yDir, zDir));
}

//!-------------------------------------------------------------------
void SpotLight::SetEdgeAngle( GLfloat angleInDeg )
{
    _edge = cosf(glm::radians(angleInDeg));
}

//!-------------------------------------------------------------------
void SpotLight::SetFlash( glm::vec3 pos, glm::vec3 dir )
{
    GLfloat lowerLight = pos.y - 0.3f;
    SetPosition(pos.x, lowerLight, pos.z);
    _direction = dir;
}

//!-------------------------------------------------------------------
SpotLight::~SpotLight()
{
}
