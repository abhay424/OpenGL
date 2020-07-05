#include "Camera.h"
#include "MyWindow.h"

//!-------------------------------------------------------------------
Camera::Camera( glm::vec3 startPosition, glm::vec3 startUp,
                GLfloat startYaw, GLfloat startPitch,
                GLfloat startMoveSpeed, GLfloat startTurnSpeed ) :
    _position(startPosition),
    _worldUp(startUp),
    _yaw(startYaw),
    _pitch(startPitch),
    _moveSpeed(startMoveSpeed),
    _turnSpeed(startTurnSpeed),
    _front(glm::vec3(0.0f, 0.0f, -1.0f)),
    _right(glm::vec3(1.0f, 0.0f, 0.0f)),
    _up(glm::vec3(0.0f, 1.0f, 0.0f))
{
    Update();
}

//!-------------------------------------------------------------------
void Camera::OnKeyPressed( MyWindow* myWindow, GLfloat deltaTime )
{
    GLfloat distance = _moveSpeed * deltaTime;

    if( myWindow->IsKeyPressed(GLFW_KEY_UP) &&
        !myWindow->IsKeyEventAccepted(GLFW_KEY_UP) )
    {
        _position += _front * distance;
        myWindow->AcceptKeyEvent(GLFW_KEY_UP);
    }

    if( myWindow->IsKeyPressed(GLFW_KEY_DOWN) &&
        !myWindow->IsKeyEventAccepted(GLFW_KEY_DOWN) )
    {
        _position -= _front * distance;
        myWindow->AcceptKeyEvent(GLFW_KEY_DOWN);
    }

    if( myWindow->IsKeyPressed(GLFW_KEY_RIGHT) &&
        !myWindow->IsKeyEventAccepted(GLFW_KEY_RIGHT) )
    {
        _position += _right * distance;
        myWindow->AcceptKeyEvent(GLFW_KEY_RIGHT);
    }

    if( myWindow->IsKeyPressed(GLFW_KEY_LEFT) &&
        !myWindow->IsKeyEventAccepted(GLFW_KEY_LEFT) )
    {
        _position -= _right * distance;
        myWindow->AcceptKeyEvent(GLFW_KEY_LEFT);
    }
}

//!-------------------------------------------------------------------
void Camera::OnMouseMoved( MyWindow* myWindow )
{
    _yaw   += (myWindow->GetXChange() * _turnSpeed);
    _pitch += (myWindow->GetYChange() * _turnSpeed);

    if( _pitch > 89.0f )
    {
        _pitch = 89.0f;
    }
    if( _pitch < -89.0f )
    {
        _pitch = -89.0f;
    }

    Update();
}

//!-------------------------------------------------------------------
void Camera::OnMouseScrolled( MyWindow* myWindow )
{
    GLfloat distance = myWindow->GetFOVChange();
    _position += _front * ( distance );
}

//!-------------------------------------------------------------------
glm::mat4 Camera::ViewMatrix()
{
    return glm::lookAt( _position, _position + _front, _up );
}

//!-------------------------------------------------------------------
void Camera::Update()
{
    _front.x = cos(glm::radians(_yaw)) * cos(glm::radians(_pitch));
    _front.y = sin(glm::radians(_pitch));
    _front.z = sin(glm::radians(_yaw)) * cos(glm::radians(_pitch));
    _front = glm::normalize(_front);

    _right = glm::normalize(glm::cross(_front, _worldUp));
    _up = glm::normalize(glm::cross(_right, _front));
}

//!-------------------------------------------------------------------
glm::vec3 Camera::Direction() const
{
    return glm::normalize(_front);
}

//!-------------------------------------------------------------------
Camera::~Camera()
{
}
