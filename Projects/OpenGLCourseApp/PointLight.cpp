
#include "PointLight.h"
#include "OmniShadowMap.h"

//!-------------------------------------------------------------------
PointLight::PointLight() :
    Light(),
    _position(0.0f, 0.0f, 0.0f),
    _aCoefficients(),
    _farPlane(0.0f)
{
}

//!-------------------------------------------------------------------
void PointLight::SetPosition( GLfloat xPos, GLfloat yPos, GLfloat zPos )
{
    _position = glm::vec3(xPos, yPos, zPos);
}

//!-------------------------------------------------------------------
void PointLight::SetAttenuationCoefficients( GLfloat constant,
                                             GLfloat linear,
                                             GLfloat exponent )
{
    _aCoefficients._constant = constant;
    _aCoefficients._linear   = linear;
    _aCoefficients._exponent = exponent;
}

//!-------------------------------------------------------------------
PointLight::AttenuationCoefficients PointLight::GetAttenuationCoefficients() const
{
    return _aCoefficients;
}

//!-------------------------------------------------------------------
void PointLight::CreateShadowMap( GLuint width, GLuint height )
{
    if( _shadowMap )
    {
        delete _shadowMap;
    }

    _shadowMap = new OmniShadowMap();
    _shadowMap->Init(width, height);
}

//!-------------------------------------------------------------------
void PointLight::SetFarAndNearPlane( GLfloat near, GLfloat far )
{
    if( !_shadowMap )
    {
        printf("Shadow map is not created yet!");
        return;
    }

    _farPlane = far;
    float aspect = (float)_shadowMap->GetWidth() / (float)_shadowMap->GetHeight();
    _lightProj = glm::perspective(glm::radians(90.0f), aspect, near, far);
}

//!-------------------------------------------------------------------
std::vector<glm::mat4> PointLight::CalculateLightTransforms()
{
    std::vector<glm::mat4> lightMatrices;

    // +x and -x. Up will be -ve y.
    lightMatrices.push_back(_lightProj * glm::lookAt(Position(), Position() + glm::vec3(1.0, 0.0, 0.0),
                                                                              glm::vec3(0.0, -1.0, 0.0)));
    lightMatrices.push_back(_lightProj * glm::lookAt(Position(), Position() + glm::vec3(-1.0, 0.0, 0.0),
                                                                              glm::vec3(0.0, -1.0, 0.0)));

    // +y and -y. Up will change now. Up will rotate to front i.e. +ve z
    lightMatrices.push_back(_lightProj * glm::lookAt(Position(), Position() + glm::vec3(0.0, 1.0, 0.0),
                                                                              glm::vec3(0.0, 0.0, 1.0)));
    lightMatrices.push_back(_lightProj * glm::lookAt(Position(), Position() + glm::vec3(0.0, -1.0, 0.0),
                                                                              glm::vec3(0.0, 0.0, -1.0)));

    // +z and -z. Up will be -ve y.
    lightMatrices.push_back(_lightProj * glm::lookAt(Position(), Position() + glm::vec3(0.0, 0.0, 1.0),
                                                                              glm::vec3(0.0, -1.0, 0.0)));
    lightMatrices.push_back(_lightProj * glm::lookAt(Position(), Position() + glm::vec3(0.0, 0.0, -1.0),
                                                                              glm::vec3(0.0, -1.0, 0.0)));

    return lightMatrices;
}

//!-------------------------------------------------------------------
PointLight::~PointLight()
{
}
