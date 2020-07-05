
#include <iostream>
#include <fstream>

#include "Shader.h"
#include "DirectionalLight.h"
#include "SpotLight.h"
#include "Material.h"
#include "ShadowMap.h"

#include <glm/gtc/type_ptr.hpp>

//!-------------------------------------------------------------------
Shader::Shader() :
    _id(0),
    _projection_mat4(0),
    _model_mat4(0),
    _view_mat4(0),
    _eye_position(0),
    _specular_intensity(0),
    _shininess(0),
    _pointLightCountLoc(0),
    _pointLightCount(0),
    _spotLightCountLoc(0),
    _spotLightCount(0),
    _texture(0),
    _dirLightShadowMap(0),
    _dirLightSpaceTransform(0),
    _omniLightPos(0),
    _omniLightFarPlane(0)
{
    for( int i = 0; i < 6; ++i )
    {
        _omniLightMatrices[i] = 0;
    }
    for( int i = 0; i < MAX_NUM_POINT_LIGHTS; ++i )
    {
        _pLightShadowMap[i]._shadowMap = 0;
        _pLightShadowMap[i]._farPlane = 0;
    }

    for( int i = 0; i < MAX_NUM_SPOT_LIGHTS; ++i )
    {
        _sLightShadowMap[i]._shadowMap = 0;
        _sLightShadowMap[i]._farPlane = 0;
    }
}

//!-------------------------------------------------------------------
std::string Shader::ReadFile( const char* shaderFile )
{
    std::string content;
    if( !shaderFile )
    {
        return content;
    }

    std::ifstream fileStream(shaderFile, std::ios::in);

    if( !fileStream.is_open() )
    {
        std::cout << "Failed to open file " << shaderFile << "." << std::endl;
    }

    std::string line = "";

    while( !fileStream.eof() )
    {
        std::getline( fileStream, line );
        content.append(line + "\n");
    }

    fileStream.close();

    return content;
}

//!-------------------------------------------------------------------
void Shader::Create( const char* vertexCode, const char* geometryCode,
                     const char* fragmentCode )
{
    Compile( vertexCode, geometryCode, fragmentCode );
}

//!-------------------------------------------------------------------
void Shader::CreateFromFile( const char* vShaderFile, const char* geomShaderFile,
                             const char* fShaderFile )
{
    std::string vCode = ReadFile(vShaderFile);
    std::string gCode = ReadFile(geomShaderFile);
    std::string fCode = ReadFile(fShaderFile);
    Compile( vCode.c_str(), ((gCode.empty()) ? nullptr: gCode.c_str()),  fCode.c_str() );
}

//!-------------------------------------------------------------------
void Shader::SetProjectionMat4( const glm::mat4& mat4 )
{
    glUniformMatrix4fv(_projection_mat4, 1, GL_FALSE, glm::value_ptr(mat4));
}

//!-------------------------------------------------------------------
void Shader::SetModelMat4( const glm::mat4& mat4 )
{
    glUniformMatrix4fv(_model_mat4, 1, GL_FALSE, glm::value_ptr(mat4));
}

//!-------------------------------------------------------------------
void Shader::SetViewMat4( const glm::mat4& mat4 )
{
    glUniformMatrix4fv(_view_mat4, 1, GL_FALSE, glm::value_ptr(mat4));
}

//!-------------------------------------------------------------------
void Shader::SetEyePosition( const glm::vec3& pos )
{
    glUniform3f(_eye_position, pos.x, pos.y, pos.z);
}

//!-------------------------------------------------------------------
void Shader::SetLight( LightBase lightLoc, const Light& light )
{
    glm::vec3 color = light.AmbientColor();
    glUniform3f(lightLoc._color, color.x, color.y, color.z);
    glUniform1f(lightLoc._ambient_intensity, light.AmbientIntensity());
    glUniform1f(lightLoc._diffuse_intensity, light.DiffuseIntensity());
}

//!-------------------------------------------------------------------
void Shader::SetPointLight( PointLightLoc lightLoc,
                            const PointLight& pLight )
{
    SetLight( lightLoc._base, pLight );

    glm::vec3 position = pLight.Position();
    glUniform3f(lightLoc._position, position.x, position.y, position.z);
    PointLight::AttenuationCoefficients coefficients = pLight.GetAttenuationCoefficients();
    glUniform1f(lightLoc._constant_coeff, coefficients._constant);
    glUniform1f(lightLoc._linear_coeff, coefficients._linear);
    glUniform1f(lightLoc._exponent_coeff, coefficients._exponent);
}

//!-------------------------------------------------------------------
void Shader::SetOmniShadowMap( OmniShadowMap omniShadowMap,
                               unsigned int textureUnit,
                               const PointLight& pLight )
{
    pLight.GetShadowMap()->Read(GL_TEXTURE0+ textureUnit);
    glUniform1i( omniShadowMap._shadowMap, textureUnit );
    glUniform1f( omniShadowMap._farPlane, pLight.GetFarPlane() );
}

//!-------------------------------------------------------------------
void Shader::SetDirectionalLight( const DirectionalLight& dLight )
{
    SetLight(_directionalLightLoc._base, dLight);
    glm::vec3 direction = dLight.Direction();
    glUniform3f(_directionalLightLoc._direction, direction.x, direction.y, direction.z);
}

//!-------------------------------------------------------------------
void Shader::SetPointLight( PointLight* pLight, unsigned int count,
                            unsigned int textureUnitOffset )
{
    if( MAX_NUM_POINT_LIGHTS <= count )
    {
        std::cout << "Count is more than the supoorted max number of point lights!" << std::endl;
        return;
    }

    _pointLightCount = count;
    glUniform1i(_pointLightCountLoc, _pointLightCount);

    for( unsigned int i = 0; i < count; ++i )
    {
        SetPointLight(_pointLights[i], pLight[i]);
        SetOmniShadowMap(_pLightShadowMap[i], textureUnitOffset + i, pLight[i]);
    }
}

//!-------------------------------------------------------------------
void Shader::SetSpotLight( SpotLight* sLight, unsigned int count,
                           unsigned int textureUnitOffset )
{
    if( MAX_NUM_SPOT_LIGHTS <= count )
    {
        std::cout << "Count is more than the supoorted max number of spot lights!" << std::endl;
        return;
    }

    _spotLightCount = count;
    glUniform1i(_spotLightCountLoc, _spotLightCount);

    for( unsigned int i = 0; i < count; ++i )
    {
        SetPointLight(_spotLights[i]._base, sLight[i]);
        SetOmniShadowMap(_sLightShadowMap[i], textureUnitOffset + i, sLight[i]);

        glm::vec3 direction = sLight[i].Direction();
        glUniform3f(_spotLights[i]._direction, direction.x, direction.y, direction.z);
        glUniform1f(_spotLights[i]._edge, sLight[i].Edge());
    }
}

//!-------------------------------------------------------------------
void Shader::SetMaterial( const Material& material )
{
    glUniform1f(_specular_intensity, material.SpecularIntensity());
    glUniform1f(_shininess, material.Shininess());
}

//!-------------------------------------------------------------------
void Shader::SetTexture( GLuint textureUnit )
{
    glUniform1i(_texture, textureUnit);
}

//!-------------------------------------------------------------------
void Shader::SetDirectionalLightShadowMap( GLuint textureUnit )
{
    glUniform1i(_dirLightShadowMap, textureUnit);
}

//!-------------------------------------------------------------------
void Shader::SetDirectionalLigthTransform( const glm::mat4& lTransform )
{
    glUniformMatrix4fv(_dirLightSpaceTransform, 1, GL_FALSE, glm::value_ptr(lTransform));
}

//!-------------------------------------------------------------------
void Shader::SetOmniLightPosition( const glm::vec3& pos )
{
    glUniform3f(_omniLightPos, pos.x, pos.y, pos.z);
}

//!-------------------------------------------------------------------
void Shader::SetOmniLightFarPlane( GLfloat farPlane )
{
    glUniform1f(_omniLightFarPlane, farPlane);
}

//!-------------------------------------------------------------------
void Shader::SetOmniLightMatrices( const std::vector<glm::mat4>& matrices )
{
    for( int i = 0; i < 6; ++i )
    {
        glUniformMatrix4fv(_omniLightMatrices[i], 1, GL_FALSE, glm::value_ptr(matrices[i]));
    }
}

//!-------------------------------------------------------------------
void Shader::Use()
{
    glUseProgram(_id);
}

//!-------------------------------------------------------------------
void Shader::Clear()
{
    if( _id != 0 )
    {
        glUseProgram(_id);
        _id = 0;
    }

    _projection_mat4 = 0;
    _model_mat4 = 0;
    _view_mat4 = 0;

    _specular_intensity = 0;
    _shininess = 0;

    Clear(_directionalLightLoc._base);
    _directionalLightLoc._direction = 0;

    for( unsigned int i = 0; i < MAX_NUM_POINT_LIGHTS; ++i )
    {
        Clear(_pointLights[i]);
    }

    for( unsigned int i = 0; i < MAX_NUM_SPOT_LIGHTS; ++i )
    {
        Clear(_spotLights[i]._base);

        _spotLights[i]._direction = 0;
        _spotLights[i]._edge = 0;
    }

    _pointLightCountLoc = 0;
    _spotLightCountLoc = 0;

    _texture = 0;
    _dirLightShadowMap = 0;
    _dirLightSpaceTransform = 0;

    _omniLightPos = 0;
    _omniLightFarPlane = 0;

    for( int i = 0; i < 6; ++i )
    {
        _omniLightMatrices[i] = 0;
    }

    for( int i = 0; i < MAX_NUM_POINT_LIGHTS; ++i )
    {
        _pLightShadowMap[i]._shadowMap = 0;
        _pLightShadowMap[i]._farPlane = 0;
    }

    for( int i = 0; i < MAX_NUM_SPOT_LIGHTS; ++i )
    {
        _sLightShadowMap[i]._shadowMap = 0;
        _sLightShadowMap[i]._farPlane = 0;
    }
}

//!-------------------------------------------------------------------
void Shader::Clear( LightBase& lightLoc )
{
    lightLoc._color = 0;
    lightLoc._ambient_intensity = 0;
    lightLoc._diffuse_intensity = 0;
}

//!-------------------------------------------------------------------
void Shader::Clear( PointLightLoc& plightLoc )
{
    Clear(plightLoc._base);

    plightLoc._position = 0;
    plightLoc._constant_coeff = 0;
    plightLoc._linear_coeff = 0;
    plightLoc._exponent_coeff = 0;
}

//!-------------------------------------------------------------------
void Shader::Compile( const char* vertexCode, const char* geometryCode,
                      const char* fragmentCode )
{
    if( !vertexCode && !geometryCode && !fragmentCode )
    {
        std::cout << "No valid shader code found." << std::endl;
        return;
    }

    _id = glCreateProgram();
    if( !_id )
    {
        std::cout << "Error creating shader program." << std::endl;
        return;
    }

    Add( vertexCode, GL_VERTEX_SHADER );
    Add( geometryCode, GL_GEOMETRY_SHADER );
    Add( fragmentCode, GL_FRAGMENT_SHADER );

    CompileProgram();
}

//!-------------------------------------------------------------------
void Shader::UpdateLocations( LightBase& lightLoc, char* initials )
{
    char bufferLoc[100] = { '\0' };
    snprintf(bufferLoc, sizeof(bufferLoc), "%s.colour", initials);
    lightLoc._color             = glGetUniformLocation(_id, bufferLoc);
    snprintf(bufferLoc, sizeof(bufferLoc), "%s.ambientIntensity", initials);
    lightLoc._ambient_intensity = glGetUniformLocation(_id, bufferLoc);
    snprintf(bufferLoc, sizeof(bufferLoc), "%s.diffuseIntensity", initials);
    lightLoc._diffuse_intensity = glGetUniformLocation(_id, bufferLoc);
}

//!-------------------------------------------------------------------
void Shader::UpdateLocations( PointLightLoc& plightLoc, char* initials )
{
    char bufferLoc[100] = { '\0' };
    snprintf(bufferLoc, sizeof(bufferLoc), "%s.base", initials);
    UpdateLocations(plightLoc._base, bufferLoc);

    snprintf(bufferLoc, sizeof(bufferLoc), "%s.position", initials);
    plightLoc._position               = glGetUniformLocation(_id, bufferLoc);

    snprintf(bufferLoc, sizeof(bufferLoc), "%s.constant_c", initials);
    plightLoc._constant_coeff         = glGetUniformLocation(_id, bufferLoc);

    snprintf(bufferLoc, sizeof(bufferLoc), "%s.linear_c", initials);
    plightLoc._linear_coeff           = glGetUniformLocation(_id, bufferLoc);

    snprintf(bufferLoc, sizeof(bufferLoc), "%s.exponent_c", initials);
    plightLoc._exponent_coeff         = glGetUniformLocation(_id, bufferLoc);
}

//!-------------------------------------------------------------------
void Shader::Add( const char* shaderCode, GLenum shaderType )
{
    if( !shaderCode )
    {
        return;
    }

    GLuint theShader = glCreateShader(shaderType);

    const GLchar* theCode[1];
    theCode[0] = shaderCode;

    GLint codeLength[1];
    codeLength[0] = strlen(shaderCode);

    glShaderSource(theShader, 1, theCode, codeLength);
    glCompileShader(theShader);

    GLint result = 0;
    GLchar eLog[1024] = { 0 };

    glGetShaderiv(theShader, GL_COMPILE_STATUS, &result);
    if( !result )
    {
        glGetShaderInfoLog(theShader, sizeof(eLog), nullptr, eLog);
        std::cout << "Error compiling the " << shaderType << " shader: "<< eLog << std::endl;
        return;
    }

    glAttachShader(_id, theShader);
}

//!-------------------------------------------------------------------
void Shader::Validate() const
{
    GLint result = 0;
    GLchar eLog[1024] = { 0 };

    glValidateProgram(_id);
    glGetProgramiv(_id, GL_VALIDATE_STATUS, &result);
    if( !result )
    {
        glGetProgramInfoLog(_id, sizeof(eLog), nullptr, eLog);
        std::cout << "Error validating program: " << eLog << std::endl;
        return;
    }
}

//!-------------------------------------------------------------------
void Shader::CompileProgram()
{
    GLint result = 0;
    GLchar eLog[1024] = { 0 };

    glLinkProgram(_id);
    glGetProgramiv(_id, GL_LINK_STATUS, &result);
    if( !result )
    {
        glGetProgramInfoLog(_id, sizeof(eLog), nullptr, eLog);
        std::cout << "Error linking program: " << eLog << std::endl;
        return;
    }

    _projection_mat4 = glGetUniformLocation(_id, "projection");
    _model_mat4      = glGetUniformLocation(_id, "model");
    _view_mat4       = glGetUniformLocation(_id, "view");

    _eye_position    = glGetUniformLocation(_id, "eyePosition");

    _specular_intensity = glGetUniformLocation(_id, "material.specularIntensity");
    _shininess          = glGetUniformLocation(_id, "material.shininess");

    char dirLightBufferLoc[100] = "directionalLight.base";
    UpdateLocations(_directionalLightLoc._base, dirLightBufferLoc);

    _directionalLightLoc._direction               = glGetUniformLocation(_id, "directionalLight.direction");

    _pointLightCountLoc = glGetUniformLocation(_id, "pointLightCount");

    for( unsigned int i = 0; i < MAX_NUM_POINT_LIGHTS; ++i )
    {
        char bufferLoc[100] = { '\0' };
        snprintf(bufferLoc, sizeof(bufferLoc), "pointLights[%d]", i);
        UpdateLocations(_pointLights[i], bufferLoc);
    }

    _spotLightCountLoc = glGetUniformLocation(_id, "spotLightCount");

    for( unsigned int i = 0; i < MAX_NUM_SPOT_LIGHTS; ++i )
    {
        char bufferLoc[100] = { '\0' };
        snprintf(bufferLoc, sizeof(bufferLoc), "spotLights[%d].base", i);
        UpdateLocations(_spotLights[i]._base, bufferLoc);

        snprintf(bufferLoc, sizeof(bufferLoc), "spotLights[%d].direction", i);
        _spotLights[i]._direction                    = glGetUniformLocation(_id, bufferLoc);

        snprintf(bufferLoc, sizeof(bufferLoc), "spotLights[%d].edge", i);
        _spotLights[i]._edge                         = glGetUniformLocation(_id, bufferLoc);
    }

    _texture = glGetUniformLocation(_id, "theTexture");
    // For directional shadow map.
    _dirLightShadowMap = glGetUniformLocation(_id, "directionalShadowMap");
    _dirLightSpaceTransform = glGetUniformLocation(_id, "dirLightSpaceTransform");

    // For omni shadow map.
    _omniLightPos = glGetUniformLocation(_id, "lightPos");
    _omniLightFarPlane = glGetUniformLocation(_id, "farPlane");
    for( int i = 0; i < 6; ++i )
    {
        char bufferLoc[100] = { '\0' };
        snprintf(bufferLoc, sizeof(bufferLoc), "lightMatrices[%d]", i);
        _omniLightMatrices[i] = glGetUniformLocation(_id, bufferLoc);
    }

    for( int i = 0; i < MAX_NUM_POINT_LIGHTS; ++i )
    {
        char bufferLoc[100] = { '\0' };
        snprintf(bufferLoc, sizeof(bufferLoc), "pLightShadowMap[%d].shadowMap", i);
        _pLightShadowMap[i]._shadowMap = glGetUniformLocation(_id, bufferLoc);

        snprintf(bufferLoc, sizeof(bufferLoc), "pLightShadowMap[%d].farPlane", i);
        _pLightShadowMap[i]._farPlane = glGetUniformLocation(_id, bufferLoc);
    }

    for( int i = 0; i < MAX_NUM_SPOT_LIGHTS; ++i )
    {
        char bufferLoc[100] = { '\0' };
        snprintf(bufferLoc, sizeof(bufferLoc), "sLightShadowMap[%d].shadowMap", i);
        _sLightShadowMap[i]._shadowMap = glGetUniformLocation(_id, bufferLoc);

        snprintf(bufferLoc, sizeof(bufferLoc), "sLightShadowMap[%d].farPlane", i);
        _sLightShadowMap[i]._farPlane = glGetUniformLocation(_id, bufferLoc);
    }
}

//!-------------------------------------------------------------------
Shader::~Shader()
{
    Clear();
}
