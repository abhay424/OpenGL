#version 330

in vec3 texCoords;

out vec4 colour;

// Wont be setting this uniform as it will have zero value.
uniform samplerCube skybox;

void main()
{
    colour = texture(skybox, texCoords);
}
