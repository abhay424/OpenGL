
#include <iostream>

#include "SkyBox.h"
#include "Shader.h"
#include "Mesh.h"

//!-------------------------------------------------------------------
SkyBox::SkyBox( const std::vector<std::string>& faceLocations ) :
    _shader(nullptr),
    _mesh(nullptr),
    _textureId(0)
{
    // Set up shader
    _shader = new Shader();
    _shader->CreateFromFile("Shaders/skybox_vertex.shader", nullptr,
                            "Shaders/skybox_fragment.shader");

    // Set up texture.
    glGenTextures(1, &_textureId);
    glBindTexture(GL_TEXTURE_CUBE_MAP, _textureId);

    int width = 0, height = 0, bitDepth = 0;
    int nbFaces = 6;
    for( int face = 0; face < nbFaces; ++face )
    {
        unsigned char* data = stbi_load( faceLocations[face].c_str(), &width, &height, &bitDepth, 0 );
        if( !data )
        {
            std::cout << "Failed to find : " << faceLocations[face].c_str() << std::endl;
            return;
        }

        glTexImage2D( GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, 0 /*MIPMAP*/,
                      GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data );

        stbi_image_free(data);
    }

    // WRAP options: GL_MIRRERED_REPEAT, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_BORDER
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // x-axis
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // y-axis
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE); // y-axis
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Mesh Set-up
    // Mesh Setup Code (to save you havin to type out the vertices of a cube!)
    unsigned int skyboxIndices[] = {
        // front
        0, 1, 2,
        2, 1, 3,
        // right
        2, 3, 5,
        5, 3, 7,
        // back
        5, 7, 4,
        4, 7, 6,
        // left
        4, 6, 0,
        0, 6, 1,
        // top
        4, 0, 5,
        5, 0, 2,
        // bottom
        1, 6, 3,
        3, 6, 7
    };

    // Since not handling lighting for skybox, normal will not calculated.
    float skyboxVertices[] = {
        -1.0f, 1.0f, -1.0f,     0.0f, 0.0f,     0.0f, 0.0f, 0.0f,
        -1.0f, -1.0f, -1.0f,    0.0f, 0.0f,     0.0f, 0.0f, 0.0f,
        1.0f, 1.0f, -1.0f,      0.0f, 0.0f,     0.0f, 0.0f, 0.0f,
        1.0f, -1.0f, -1.0f,     0.0f, 0.0f,     0.0f, 0.0f, 0.0f,

        -1.0f, 1.0f, 1.0f,      0.0f, 0.0f,     0.0f, 0.0f, 0.0f,
        1.0f, 1.0f, 1.0f,       0.0f, 0.0f,     0.0f, 0.0f, 0.0f,
        -1.0f, -1.0f, 1.0f,     0.0f, 0.0f,     0.0f, 0.0f, 0.0f,
        1.0f, -1.0f, 1.0f,      0.0f, 0.0f,     0.0f, 0.0f, 0.0f
    };

    _mesh = new Mesh();
    _mesh->Create(skyboxVertices, 64, skyboxIndices, 36, true);
}

//!-------------------------------------------------------------------
void SkyBox::Draw( const glm::mat4& viewMatrix,
                   const glm::mat4& projectionMatrix )
{
    glDepthMask(GL_FALSE);

    _shader->Use();

    _shader->SetProjectionMat4(projectionMatrix);
    // Covert view matrix to 3x3 matrix to remove translation.
    glm::mat4 viewMat4 = glm::mat4(glm::mat3(viewMatrix));
    _shader->SetViewMat4(viewMat4);

    glActiveTexture(GL_TEXTURE0); // since this is the only texture being drawn
    glBindTexture(GL_TEXTURE_CUBE_MAP, _textureId);
    // Will be using default zero value for samplerCube.

    _shader->Validate();
    _mesh->Render();

    glDepthMask(GL_TRUE);
}

//!-------------------------------------------------------------------
SkyBox::~SkyBox()
{
}
