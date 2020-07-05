
#ifndef _MYWINDOW_
#define _MYWINDOW_

#include <stdio.h>
#include <bitset>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>


class MyWindow
{
public:
    MyWindow( GLint width = (GLint)800, GLint height = (GLint)600 );

    int Initialize();

    void SetBackgroundColor(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha = 1.0f);
    void SetViewport( GLint width, GLint height );

    GLfloat BufferWidth() const { return (GLfloat)_bufferWidth; }
    GLfloat BufferHeight() const { return (GLfloat)_bufferHeight; }

    void AcceptKeyEvent(size_t key);
    bool IsKeyEventAccepted(size_t key) const { return _keyAcceptStates.test(key); }

    bool IsKeyPressed( size_t key ) const { return _keys.test(key); }
    bool IsKeyReleased( size_t key ) const { return !IsKeyPressed(key); }

    bool ShouldClose() const;

    void SwapBuffer() const;

    GLfloat GetXChange();
    GLfloat GetYChange();
    GLfloat GetFOVChange();

    glm::mat4 GetProjectionMatrix();

    ~MyWindow();

private:

    void RegisterEventHandlers();

    static void HandleKeyEvent( GLFWwindow* window, int key, int code,
                                                    int action, int mode );

    static void HandleMouseEvent( GLFWwindow* window, double xPos, double yPos );

    static void HandleMouseScollEvent( GLFWwindow* window, double xOffset, double yOffset );

private:
    GLFWwindow* _mainWindow;

    GLint _width, _height;
    GLint _bufferWidth, _bufferHeight;

    std::bitset<1024> _keys;
    std::bitset<1024> _keyAcceptStates;

    // Mouse stored values.
    GLfloat _lastX, _lastY;
    GLfloat _xChange, _yChange;
    bool    _firstMouseMove;

    GLfloat _fov, _lastFov; // Field of view;
};

#endif //_MYWINDOW_
