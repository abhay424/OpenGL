
#ifndef _SHADER_
#define _SHADER_


#include <stdio.h>
#include <string>
#include <vector>

#include <GL/glew.h>
#include <glm/glm.hpp>

#define MAX_NUM_POINT_LIGHTS   3
#define MAX_NUM_SPOT_LIGHTS    3

class Light;
class DirectionalLight;
class PointLight;
class SpotLight;
class Material;
class ShadowMap;

class Shader
{
public:
    Shader();

    void Create( const char* vertexCode, const char* geometryCode,
                 const char* fragmentCode );
    void CreateFromFile( const char* vShaderFile, const char* geomShaderFile,
                         const char* fShaderFile );

    void Validate() const;

    void SetProjectionMat4( const glm::mat4 &mat4 );
    void SetModelMat4( const glm::mat4 &mat4 );
    void SetViewMat4( const glm::mat4 &mat4 );

    void SetEyePosition( const glm::vec3 &pos );

    void SetDirectionalLight( const DirectionalLight& dLight );
    void SetPointLight( PointLight* pLight, unsigned int count,
                        unsigned int textureUnit );
    void SetSpotLight( SpotLight* sLight, unsigned int count,
                       unsigned int textureUnit );

    void SetMaterial( const Material& material );

    void SetTexture(GLuint textureUnit);
    void SetDirectionalLightShadowMap(GLuint textureUnit);
    void SetDirectionalLigthTransform(const glm::mat4& lTransform);

    void SetOmniLightPosition(const glm::vec3& pos);
    void SetOmniLightFarPlane(GLfloat farPlane);
    void SetOmniLightMatrices(const std::vector<glm::mat4>& matrices);

    void Use();
    void Clear();

    ~Shader();

private:

    struct LightBase
    {
        GLuint  _color;
        GLuint  _ambient_intensity,
                _diffuse_intensity;
    };
    // Locations for Directional light.
    struct DirectionalLightLoc
    {
        LightBase   _base;
        GLuint      _direction;
    };
    // Locations for Point light.
    struct PointLightLoc
    {
        LightBase   _base;

        GLuint      _position;
        GLuint      _constant_coeff,
                    _linear_coeff,
                    _exponent_coeff;
    };
    // Locations for Spot light.
    struct SpotLightLoc
    {
        PointLightLoc _base;

        GLuint        _direction;
        GLuint        _edge;
    };

    struct OmniShadowMap
    {
        GLuint        _shadowMap,
                      _farPlane;
    };

    void SetLight( LightBase lightLoc, const Light& light );
    void SetPointLight( PointLightLoc lightLoc,
                        const PointLight& pLight );
    void SetOmniShadowMap( OmniShadowMap omniShadowMap,
                           unsigned int textureUnitOffset,
                           const PointLight& pLight );

    void UpdateLocations( LightBase &lightLoc, char* initials );
    void UpdateLocations( PointLightLoc &plightLoc, char* initials );

    void Clear(LightBase &lightLoc);
    void Clear(PointLightLoc &plightLoc);

    std::string ReadFile( const char* shaderFile );

    void Compile( const char* vertexCode, const char* geometryCode, const char* fragmentCode );
    void Add( const char* shaderCode, GLenum shaderType );
    void CompileProgram(); // To update locations.

    GLuint      _id; // Shader id.

    // Uniforms locations.
    // Matrix locations.
    GLuint      _projection_mat4,
                _model_mat4,
                _view_mat4;
    // Eye position, i.e. camera position uniform.
    GLuint      _eye_position;
    // Uniform locations for materials.
    GLuint      _specular_intensity,
                _shininess;

    DirectionalLightLoc _directionalLightLoc;

    PointLightLoc   _pointLights[MAX_NUM_POINT_LIGHTS];

    GLuint          _pointLightCountLoc;
    unsigned int    _pointLightCount;

    SpotLightLoc    _spotLights[MAX_NUM_SPOT_LIGHTS];

    GLuint          _spotLightCountLoc;
    unsigned int    _spotLightCount;

    // Uniform for texture
    GLuint      _texture;                    // In fragment.shader.

    // Uniforms for directional light shadow.
    GLuint      _dirLightShadowMap,         // In fragment.shader.
                _dirLightSpaceTransform;    // In directional_shadow_map_vertex.shader.

    // Uniforms for omni-directional light shadow map.
    GLuint      _omniLightPos,              // In omni_shadow_map_fragment.shader.
                _omniLightFarPlane;         // In omni_shader_map_fragment.shader
    GLuint      _omniLightMatrices[6];      // In omni_shadow_map_geometry.shader.

    // Uniform locations for omni point light shadow maps in fragment.shader
    OmniShadowMap   _pLightShadowMap[MAX_NUM_POINT_LIGHTS];
    // Uniform locations for omni spot light shadow maps in fragment.shader
    OmniShadowMap   _sLightShadowMap[MAX_NUM_SPOT_LIGHTS];
};

#endif // !_SHADER_
