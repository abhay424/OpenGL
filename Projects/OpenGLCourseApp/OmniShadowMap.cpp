
#include <stdio.h>

#include "OmniShadowMap.h"

//!-------------------------------------------------------------------
OmniShadowMap::OmniShadowMap()
    : ShadowMap()
{
}

//!-------------------------------------------------------------------
bool OmniShadowMap::Init( GLuint width, GLuint height )
{
    _shadowWidth = width;
    _shadowHeight = height;

    glGenFramebuffers(1, &_fbo);

    glGenTextures(1, &_shadowMap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, _shadowMap);

    for( size_t i = 0; i < 6; ++i )
    {
        // Iterate for all six faces as these enums values are continuous.
        // GL_TEXTURE_CUBE_MAP_POSITIVE_X, GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
        // GL_TEXTURE_CUBE_MAP_POSITIVE_Y, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
        // GL_TEXTURE_CUBE_MAP_POSITIVE_Z, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0,
                     GL_DEPTH_COMPONENT, _shadowWidth, _shadowHeight,
                     0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // x-axis
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // y-axis
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE); // z-axis

    glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, _shadowMap, 0);

    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if( status != GL_FRAMEBUFFER_COMPLETE )
    {
        printf( "FrameBuffer Error! : %i\n", status );
        return false;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return true;
}

//!-------------------------------------------------------------------
void OmniShadowMap::Write()
{
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _fbo);
}

//!-------------------------------------------------------------------
void OmniShadowMap::Read( GLenum textureUnit )
{
    glActiveTexture(textureUnit);
    glBindTexture(GL_TEXTURE_CUBE_MAP, _shadowMap);
}

//!-------------------------------------------------------------------
OmniShadowMap::~OmniShadowMap()
{
}
