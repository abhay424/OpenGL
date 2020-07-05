
#ifndef _SPOTLIGHT_
#define _SPOTLIGHT_

#include "PointLight.h"

// Spot Light class.
// Type of Point light but with defined angle.
class SpotLight : public PointLight
{
public:

    SpotLight();

    void SetDirection( GLfloat xDir, GLfloat yDir, GLfloat zDir );
    inline glm::vec3 Direction() const { return _direction; };

    void SetEdgeAngle( GLfloat angleInDeg );
    inline GLfloat Edge() const { return _edge; }

    void SetFlash( glm::vec3 pos, glm::vec3 dir );

    ~SpotLight();

private:

    glm::vec3   _direction;
    GLfloat     _edge; // Cosine of angle

};

#endif // !_SPOTLIGHT_
