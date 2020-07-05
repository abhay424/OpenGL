
#include "Light.h"
#include "ShadowMap.h"

//!-------------------------------------------------------------------
Light::Light() :
    _color( 1.0f, 1.0f, 1.0f ),
    _ambientIntensity(1.0f),
    _diffuseIntensity( 0.0f ),        // By default no diffuse
    _isAmbientLightON(true),
    _isDiffuseLightON(true),
    _shadowMap(nullptr)
{
}

//!-------------------------------------------------------------------
glm::vec3 Light::AmbientColor(void) const
{
    return _color;
}

//!-------------------------------------------------------------------
void Light::SetAmbientColor( GLfloat red, GLfloat green, GLfloat blue )
{
    _color = glm::vec3(red, green, blue);
}

//!-------------------------------------------------------------------
GLfloat Light::AmbientIntensity(void) const
{
    if( !IsAmbientLightOn() )
    {
        return 0.0f;
    }
    return _ambientIntensity;
}

//!-------------------------------------------------------------------
void Light::SetAmbientIntensity( GLfloat aIntensity )
{
    _ambientIntensity = aIntensity;
}

//!-------------------------------------------------------------------
GLfloat Light::DiffuseIntensity(void) const
{
    if( !IsDiffuseLightOn() )
    {
        return 0.0f;
    }

    return _diffuseIntensity;
}

//!-------------------------------------------------------------------
void Light::SetDiffuseIntensity( GLfloat dIntensity )
{
    _diffuseIntensity = dIntensity;
}

//!-------------------------------------------------------------------
void Light::CreateShadowMap( GLuint width, GLuint height )
{
    if( _shadowMap )
    {
        delete _shadowMap;
    }

    _shadowMap = new ShadowMap();
    _shadowMap->Init(width, height);
}

//!-------------------------------------------------------------------
void Light::SetOn( bool state )
{
    SetAmbientLightState(state);
    SetDiffuseLightState(state);
}

////!-------------------------------------------------------------------
//void Light::UseAmbient( GLuint intensityLoc, GLuint colorLoc )
//{
//    glUniform3f( colorLoc, _color.x, _color.y, _color.z );
//    glUniform1f( intensityLoc, _ambientIntensity );
//}
//
////!-------------------------------------------------------------------
//void Light::UseDiffuse( GLuint intensityLoc, GLuint directioLoc )
//{
//    glUniform3f( directioLoc, _direction.x, _direction.y, _direction.z );
//    glUniform1f( intensityLoc, _ambientIntensity );
//}

//!-------------------------------------------------------------------
Light::~Light()
{
    if( _shadowMap )
    {
        delete _shadowMap;
    }
}
