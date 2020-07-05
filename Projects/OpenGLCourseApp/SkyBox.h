
#ifndef _SKYBOX_
#define _SKYBOX_


#include <vector>
#include <string>
#include "CommonIncludes.h"

#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Shader;
class Mesh;


class SkyBox
{
public:

    SkyBox( const std::vector<std::string>& faceLocations );

    void Draw(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix);

    ~SkyBox();

private:

    Shader*     _shader;
    Mesh*       _mesh;

    GLuint      _textureId;

};

#endif // !_SKYBOX_
