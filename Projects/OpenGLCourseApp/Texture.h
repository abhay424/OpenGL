
#ifndef _TEXTURE_
#define _TEXTURE_

#include "CommonIncludes.h"

#include <GL/glew.h>

class Texture
{
public:

    Texture( const char* fileLoc = nullptr );

    bool Load(bool withAlpha = true);
    void Use();
    void Clear();

    ~Texture();

private:

    GLuint          _id;
    int             _width,
                    _height,
                    _bitDepth;

    const char*     _fileLoc;

};

#endif // !_TEXTURE_
