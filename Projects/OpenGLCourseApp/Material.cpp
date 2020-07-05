#include "Material.h"

//!-------------------------------------------------------------------
Material::Material( GLfloat sInensity, GLfloat shininess )
    : _specularIntensity(sInensity),
      _shininess(shininess)
{
}

//!-------------------------------------------------------------------
void Material::SetSpecularIntensity( GLfloat sInensity )
{
    _specularIntensity = sInensity;
}

//!-------------------------------------------------------------------
void Material::SetShininess( GLfloat shininess )
{
    _shininess = shininess;
}

//!-------------------------------------------------------------------
Material::~Material()
{
}
