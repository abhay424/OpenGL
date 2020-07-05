
#ifndef _CAMERA_
#define _CAMERA_

#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class MyWindow;

class Camera
{
public:
    Camera( glm::vec3 startPosition = glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3 startUp = glm::vec3(0.0f, 1.0f, 0.0f),
            GLfloat startYaw = -60.0f, GLfloat startPitch = 0.0f,
            GLfloat startMoveSpeed = 5.0f, GLfloat startTurnSpeed = 0.50f );

    void OnKeyPressed( MyWindow *myWindow, GLfloat deltaTime );
    void OnMouseMoved( MyWindow *myWindow );
    void OnMouseScrolled( MyWindow *myWindow );

    glm::mat4 ViewMatrix();

    glm::vec3 Position() const { return _position; }
    glm::vec3 Direction() const;

    ~Camera();

private:

    void Update();

private:

    glm::vec3   _position,
                _front,
                _up,
                _right,
                _worldUp;

    GLfloat     _yaw,
                _pitch;

    GLfloat     _moveSpeed,
                _turnSpeed;

};

#endif // !_CAMERA_
