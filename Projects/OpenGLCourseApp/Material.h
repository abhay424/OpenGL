
#ifndef _MATERIAL_
#define _MATERIAL_


#include <GL/glew.h>

class Material
{
public:

    Material( GLfloat sInensity = 0.0f, GLfloat shininess = 0.0f );

    void SetSpecularIntensity( GLfloat sInensity );
    inline GLfloat SpecularIntensity() const { return _specularIntensity; }

    void SetShininess( GLfloat shininess );
    inline GLfloat Shininess() const { return _shininess; }

    ~Material();

private:

    GLfloat     _specularIntensity,
                _shininess;

};

#endif // !_MATERIAL_
