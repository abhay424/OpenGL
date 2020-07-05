
#include <iostream>

#include "Texture.h"

//!-------------------------------------------------------------------
Texture::Texture( const char* fileLoc ) :
    _fileLoc(fileLoc),
    _id(0),
    _width(0),
    _height(0),
    _bitDepth(0)
{
}

//!-------------------------------------------------------------------
bool Texture::Load(bool withAlpha)
{
    if( !_fileLoc )
    {
        std::cout << "No valid file location found!" << std::endl;
        return false;
    }

    unsigned char* data = stbi_load( _fileLoc, &_width, &_height, &_bitDepth, 0 );
    if( !data )
    {
        std::cout << "Failed to find : " << _fileLoc << std::endl;
    }

    glGenTextures(1, &_id);
    glBindTexture(GL_TEXTURE_2D, _id);

    // WRAP options: GL_MIRRERED_REPEAT, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_BORDER
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // x-axis
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); // y-axis
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0 /*MIPMAP*/, (withAlpha? GL_RGBA : GL_RGB), _width, _height, 0,
                                              (withAlpha? GL_RGBA : GL_RGB), GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, 0);

    stbi_image_free(data);
    return true;
}

//!-------------------------------------------------------------------
void Texture::Use()
{
    glActiveTexture(GL_TEXTURE1); // Texture unit.
    glBindTexture(GL_TEXTURE_2D, _id);
}

//!-------------------------------------------------------------------
void Texture::Clear()
{
    if( _id != 0 )
    {
        glDeleteTextures(1, &_id);
    }

    _id = 0;
    _width = 0;
    _height = 0;
    _bitDepth = 0;
    _fileLoc = nullptr;
}

//!-------------------------------------------------------------------
Texture::~Texture()
{
    Clear();
}
