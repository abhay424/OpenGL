#version 330

layout(location = 0) in vec3 pos;

uniform mat4 model;
uniform mat4 dirLightSpaceTransform;

void main()
{
    gl_Position = dirLightSpaceTransform * model * vec4(pos, 1.0);
}
