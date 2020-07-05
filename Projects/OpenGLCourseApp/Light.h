
#ifndef _LIGHT_
#define _LIGHT_

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class ShadowMap;

class Light
{
public:
    Light();

    glm::vec3 AmbientColor(void) const;
    void SetAmbientColor( GLfloat red = 1.0f,
                          GLfloat green = 1.0f,
                          GLfloat blue = 1.0f );

    GLfloat AmbientIntensity(void) const;
    void SetAmbientIntensity( GLfloat aIntensity = 1.0f );

    GLfloat DiffuseIntensity(void) const;
    void SetDiffuseIntensity( GLfloat dIntensity = 0.0f );

    virtual void CreateShadowMap( GLuint width, GLuint height );
    inline ShadowMap* GetShadowMap() const { return _shadowMap; }

    void Toggle() { SetOn(!IsOn()); }

    void SetOn( bool state );
    bool IsOn() const { return IsAmbientLightOn() || IsDiffuseLightOn(); }

    void SetAmbientLightState(bool state) { _isAmbientLightON = state; }
    inline bool IsAmbientLightOn() const { return _isAmbientLightON; }

    void SetDiffuseLightState( bool state ) { _isDiffuseLightON = state; }
    inline bool IsDiffuseLightOn() const { return _isDiffuseLightON; }

    virtual ~Light();

protected:

    glm::vec3   _color;
    GLfloat     _ambientIntensity;
    GLfloat     _diffuseIntensity;

    glm::mat4   _lightProj;
    ShadowMap*  _shadowMap;

    bool        _isAmbientLightON,
                _isDiffuseLightON;
};

#endif // !_LIGHT_
