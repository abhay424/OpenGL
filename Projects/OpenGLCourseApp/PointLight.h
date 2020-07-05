
#ifndef _POINTLIGHT_
#define _POINTLIGHT_

#include <vector>

#include "Light.h"

// Point light class.
// It is light which emits light in all direction i.e. 360 around a point.
class PointLight : public Light
{
public:
    PointLight();

    inline glm::vec3 Position() const { return _position; }
    void SetPosition( GLfloat xPos,
                      GLfloat yPos,
                      GLfloat zPos );

    void SetAttenuationCoefficients( GLfloat constant,
                                     GLfloat linear,
                                     GLfloat exponent );

    struct AttenuationCoefficients
    {
        AttenuationCoefficients() :
            _constant(1.0f),
            _linear(1.0f),
            _exponent(1.0f) {}
        // Values to calculate attenuation.
        // attenuation = 1/(_exponent*x^2 + _linear*x + _constant),
        // Where x is how far fragPos is from source of light.
        GLfloat     _constant,
                    _linear,
                    _exponent;
    };

    AttenuationCoefficients GetAttenuationCoefficients() const;

    std::vector<glm::mat4> CalculateLightTransforms();
    void CreateShadowMap( GLuint width, GLuint height ) override;

    void SetFarAndNearPlane( GLfloat near, GLfloat far );
    inline GLfloat GetFarPlane() const { return _farPlane; }

    ~PointLight();

private:

    glm::vec3               _position; // Position of the light source.

    AttenuationCoefficients _aCoefficients;

    GLfloat                 _farPlane;
};

#endif // !_POINTLIGHT_
