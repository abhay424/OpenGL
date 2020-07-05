
#include <stdio.h>

#include "ShadowMap.h"

//!-------------------------------------------------------------------
ShadowMap::ShadowMap()
    : _fbo(0),
      _shadowMap(0),
      _shadowWidth(0),
      _shadowHeight(0)
{
}

//!-------------------------------------------------------------------
bool ShadowMap::Init( GLuint width, GLuint height )
{
    _shadowWidth = width;
    _shadowHeight = height;

    glGenFramebuffers(1, &_fbo);

    glGenTextures(1, &_shadowMap);
    glBindTexture(GL_TEXTURE_2D, _shadowMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, _shadowWidth, _shadowHeight,
                 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    // WRAP options: GL_MIRRERED_REPEAT, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_BORDER
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER); // x-axis
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER); // y-axis
    float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, _shadowMap, 0);

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
void ShadowMap::Write()
{
    glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
}

//!-------------------------------------------------------------------
void ShadowMap::Read( GLenum textureUnit )
{
    glActiveTexture(textureUnit);
    glBindTexture(GL_TEXTURE_2D, _shadowMap);
}

//!-------------------------------------------------------------------
ShadowMap::~ShadowMap()
{
    if( _fbo != 0 )
    {
        glDeleteFramebuffers(1, &_fbo);
        _fbo = 0;
    }
    if( _shadowMap != 0 )
    {
        glDeleteTextures(1, &_shadowMap);
        _shadowMap = 0;
    }
}
