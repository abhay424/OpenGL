
#ifndef _DIRECTIONALLIGHT_
#define _DIRECTIONALLIGHT_

#include "Light.h"


class DirectionalLight : public Light
{
public:
    DirectionalLight();

    glm::vec3 Direction() const;
    void SetDirection( GLfloat xDir = 0.0f,
                       GLfloat yDir = -1.0f,
                       GLfloat zDir = 0.0f );

    glm::mat4 CalculateLightTransform() const;

    ~DirectionalLight();

private:

    glm::vec3   _direction; // Has fixed light direction.

};

#endif // !_DIRECTIONALLIGHT_
