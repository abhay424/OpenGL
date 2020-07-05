#version 330

in vec4 fragPos;

uniform vec3 lightPos;
uniform float farPlane;

void main()
{
    float distance = length(fragPos.xyz - lightPos);
    gl_FragDepth = distance / farPlane;
}
