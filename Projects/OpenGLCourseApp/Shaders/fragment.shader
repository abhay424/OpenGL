#version 330

in vec4 vCol;
in vec2 texCoord;
in vec3 normal;
in vec3 fragPos;
in vec4 DirectionalLightSpacePos;

out vec4 colour;

const int MAX_NUM_POINT_LIGHTS = 3;
const int MAX_NUM_SPOT_LIGHTS = 3;

struct LightBase
{
    vec3  colour;
    float ambientIntensity;
    float diffuseIntensity;
};

struct DirectionalLight
{
    LightBase base;
    vec3  direction;
};

struct PointLight
{
    LightBase base;

    vec3 position;
    float constant_c;
    float linear_c;
    float exponent_c;
};

struct SpotLight
{
    PointLight base;

    vec3 direction;
    float edge;
};

struct OmniShadowMap
{
    samplerCube shadowMap;
    float farPlane;
};

struct Material
{
    float specularIntensity;
    float shininess;
};

uniform int pointLightCount;
uniform int spotLightCount;

uniform DirectionalLight directionalLight;
uniform PointLight pointLights[MAX_NUM_POINT_LIGHTS];
uniform SpotLight spotLights[MAX_NUM_SPOT_LIGHTS];

uniform sampler2D theTexture;
uniform sampler2D directionalShadowMap;
uniform OmniShadowMap pLightShadowMap[MAX_NUM_POINT_LIGHTS];
uniform OmniShadowMap sLightShadowMap[MAX_NUM_SPOT_LIGHTS];

uniform Material material;

uniform vec3 eyePosition;

// Having predefined samples for omni light pcf calculation
vec3 sampleOffsetDirections[20] = vec3[]
(
    vec3(1, 1, 1), vec3(1, -1, 1), vec3(-1, -1, 1), vec3(-1, 1, 1),
    vec3(1, 1, -1), vec3(1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
    vec3(1, 1, 0), vec3(1, -1, 0), vec3(-1, -1, 0), vec3(-1, 1, 0),
    vec3(1, 0, 1), vec3(-1, 0, 1), vec3(1, 0, -1), vec3(-1, 0, -1),
    vec3(0, 1, 1), vec3(0, -1, 1), vec3(0, -1, -1), vec3(0, 1, -1)
);

float CalcDirectionalShadowFactor(DirectionalLight light)
{
    vec3 projCoords = DirectionalLightSpacePos.xyz / DirectionalLightSpacePos.w;
    projCoords = (projCoords * 0.5) + 0.5;

    float currentDepth = projCoords.z;

    float shadow = 0.0f;
    if( currentDepth <= 1.0f )
    {
        float shadowFactor = dot(normalize(normal), normalize(light.direction));
        float bias = max(0.05 * (1 - shadowFactor), 0.005);

        vec2 texelSize = 1.0 / textureSize(directionalShadowMap, 0);

        for( int x = -1; x <= 1; ++x )
        {
            for( int y = -1; y <= 1; ++y )
            {
                float pcfDepth = texture(directionalShadowMap, projCoords.xy + (vec2(x, y) * texelSize)).r;
                shadow += ((currentDepth - bias) > pcfDepth) ? 1.0f : 0.0f;
            }
        }

        shadow /= 9.0f;
    }

    return shadow;
}

float CalculateOmniShadowFactor(PointLight pLight, OmniShadowMap omniShadowMap)
{
    vec3 fragToLight = fragPos - pLight.position;
    float currentDepth = length(fragToLight);
    //float closestDepth = texture(omniShadowMap.shadowMap, fragToLight).r;
    //closestDepth *= omniShadowMap.farPlane;

    //float bias = 0.05f;
    //float shadowFactor = (currentDepth - bias) > closestDepth ? 1.0f : 0.0f;

    float shadowFactor = 0.0f;
    float bias = 0.05;
    //float samples = 4.0f;
    //float offset = 0.1f;
    //for( float x = -offset; x < offset; x += (offset / (samples * 0.5)) )
    //{
    //    for( float y = -offset; y < offset; y += (offset / (samples * 0.5)) )
    //    {
    //        for( float z = -offset; z < offset; z += (offset / (samples * 0.5)) )
    //        {
    //            float closestDepth = texture(omniShadowMap.shadowMap, fragToLight + vec3(x, y, z)).r;
    //            closestDepth *= omniShadowMap.farPlane;
    //            if( (currentDepth - bias) > closestDepth )
    //            {
    //                shadowFactor += 1.0f;
    //            }
    //        }
    //    }
    //}
    //shadowFactor /= (samples * samples * samples);

    int samples = 20;

    // calculate disk radius based on how far camera is.
    float viewDistance = length(eyePosition - fragPos);
    float diskRadius = (1.0 + (viewDistance / omniShadowMap.farPlane)) / 25.0f;

    //float diskRadius = 0.05f;
    for( int i = 0; i < samples; ++i )
    {
        float closestDepth = texture(omniShadowMap.shadowMap, fragToLight + sampleOffsetDirections[i] * diskRadius).r;
        closestDepth *= omniShadowMap.farPlane;
        if( (currentDepth - bias) > closestDepth )
        {
            shadowFactor += 1.0f;
        }
    }
    shadowFactor /= float(samples);

    return shadowFactor;
}

vec4 CalculateLightByDirection(LightBase base, vec3 direction, float shadow)
{
    vec4 ambientColor = vec4(base.colour, 1.0f) * base.ambientIntensity;

    float diffuseFactor = max(dot(normalize(normal), normalize(direction)), 0.0f);
    vec4 diffuseColor = vec4(base.colour, 1.0f) * base.diffuseIntensity * diffuseFactor;

    vec4 specularColor = vec4(0.0f, 0.0f, 0.0f, 0.0f);
    if( diffuseFactor > 0.0f )
    {
        vec3 fragToEye = normalize(eyePosition - fragPos);
        vec3 reflectionVector = reflect(normalize(direction), normalize(normal));

        float specularFactor = dot(normalize(fragToEye), normalize(reflectionVector));
        if( specularFactor > 0.0f )
        {
            specularFactor = pow(specularFactor, material.shininess);
            specularColor = vec4(base.colour * material.specularIntensity * specularFactor, 1.0f);
        }
    }

    return (ambientColor + (1.0 - shadow) * (diffuseColor + specularColor));
}

vec4 CalculateDirectionalLight()
{
    float shadow = CalcDirectionalShadowFactor(directionalLight);
    return CalculateLightByDirection(directionalLight.base, directionalLight.direction, shadow);
}

vec4 CalculatePointLight(PointLight pLight, OmniShadowMap omniShadowMap)
{
    vec3 direction = fragPos - pLight.position;
    float distance = length(direction);
    direction = normalize(direction);

    float shadowFactor = CalculateOmniShadowFactor(pLight, omniShadowMap);

    vec4 color = CalculateLightByDirection(pLight.base, direction, shadowFactor);
    float attenuation = pLight.exponent_c * distance * distance +
                        pLight.linear_c * distance +
                        pLight.constant_c;
    return (color / attenuation);
}

vec4 CalculatePointLights()
{
    vec4 totalColor = vec4(0.0f, 0.0f, 0.0f, 0.0f);
    for( int i = 0; i < pointLightCount; i++ )
    {
        totalColor += CalculatePointLight(pointLights[i], pLightShadowMap[i]);
    }

    return totalColor;
}

vec4 CalculateSpotLight(SpotLight sLight, OmniShadowMap omniShadowMap)
{
    vec4 color = vec4(0.0f, 0.0f, 0.0f, 0.0f);
    vec3 lightRay = fragPos - sLight.base.position;
    lightRay = normalize(lightRay);

    float slFactor = dot(lightRay, normalize(sLight.direction));
    if( sLight.edge < slFactor )
    {
        color = CalculatePointLight(sLight.base, omniShadowMap);

        float factor = (slFactor - sLight.edge) / (1.0f - sLight.edge);
        //float factor = 1.0f - ((1.0f - slFactor) * ( 1.0f / (1.0f - sLight.edge)));
        color *= factor;
    }
    return color;
}

vec4 CalculateSpotLights()
{
    vec4 totalColor = vec4(0.0f, 0.0f, 0.0f, 0.0f);
    for( int i = 0; i < spotLightCount; i++ )
    {
        totalColor += CalculateSpotLight(spotLights[i], sLightShadowMap[i]);
    }

    return totalColor;
}

void main()
{
    vec4 finalColor = CalculateDirectionalLight();
    finalColor += CalculatePointLights();
    finalColor += CalculateSpotLights();

    colour = texture(theTexture, texCoord) * finalColor;
}
