
#ifndef _SHADOWMAP_
#define _SHADOWMAP_


#include <GL\glew.h>

class ShadowMap
{
public:
    ShadowMap();

    virtual bool Init( GLuint width, GLuint height );
    void Write();
    virtual void Read( GLenum textureUnit );

    inline GLuint GetWidth() const { return _shadowWidth; }
    inline GLuint GetHeight() const { return _shadowHeight; }

    virtual ~ShadowMap();

protected:

    GLuint  _fbo,
            _shadowMap;

    GLuint  _shadowWidth,
            _shadowHeight;

};

#endif // !_SHADOWMAP_
