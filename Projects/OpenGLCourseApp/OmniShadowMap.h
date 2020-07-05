
#ifndef _OMNISHADOWMAP_
#define _OMNISHADOWMAP_

#include "ShadowMap.h"

class OmniShadowMap : public ShadowMap
{
public:

    OmniShadowMap();

    bool Init( GLuint width, GLuint height ) override;
    void Write();
    void Read( GLenum textureUnit ) override;

    ~OmniShadowMap();

};

#endif // !_OMNISHADOWMAP_
