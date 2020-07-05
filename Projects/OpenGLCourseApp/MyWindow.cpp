
#include <iostream>

#include "MyWindow.h"

//!-------------------------------------------------------------------
MyWindow::MyWindow( GLint width, GLint height ) :
    _mainWindow(nullptr),
    _width(width),
    _height(height),
    _bufferWidth(0),
    _bufferHeight(0),
    _firstMouseMove(true),
    _lastX(0.0f), _lastY(0.0f),
    _xChange(0.0f), _yChange(0.0f),
    _fov(60.0f), _lastFov(60.f)
{
    _keyAcceptStates.flip();
}

//!-------------------------------------------------------------------
int MyWindow::Initialize()
{
    // Initialize GLFW
    if( !glfwInit() )
    {
        std::cout << "GLFW initialization failed." << std::endl;
        glfwTerminate();
        return 0;
    }

    // Set up GLFW window properties.
    // OpenGL version
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    // Core profile = No Backwards compatibility
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // Allow forward compatibility
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    _mainWindow = glfwCreateWindow(_width, _height, "Test Window", nullptr, nullptr);
    if( !_mainWindow )
    {
        std::cout << "GLFW window creation failed." << std::endl;
        glfwTerminate();
        return 0;
    }

    // Get Buffer size information
    glfwGetFramebufferSize(_mainWindow, &_bufferWidth, &_bufferHeight);

    // Set context for GLFW to use
    glfwMakeContextCurrent(_mainWindow);

    // Register key + mouse input event handlers;
    RegisterEventHandlers();
    //glfwSetInputMode(_mainWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Allow modern extension features
    glewExperimental = GL_TRUE;

    if( glewInit() != GLEW_OK )
    {
        std::cout << "GLEW initialization failed." << std::endl;
        // Destroy created GLFW window.
        glfwDestroyWindow(_mainWindow);
        glfwTerminate();
        return 0;
    }

    glEnable(GL_DEPTH_TEST);

    // Setup Viewport size.
    // Actual GL call, not from GLFW.
    // Will not be actual WIDTH, HEIGHT, instead they are buffer width and height returned by main window.
    glViewport(0, 0, _bufferWidth, _bufferHeight);

    glfwSetWindowUserPointer(_mainWindow, this);

    return 1;
}

//!-------------------------------------------------------------------
void MyWindow::SetBackgroundColor( GLfloat red, GLfloat green,
                                   GLfloat blue, GLfloat alpha )
{
    glClearColor(red, green, blue, alpha);
}

//!-------------------------------------------------------------------
void MyWindow::SetViewport( GLint width, GLint height )
{
    glViewport(0, 0, width, height);
}

//!-------------------------------------------------------------------
void MyWindow::AcceptKeyEvent( size_t key )
{
    _keyAcceptStates.set(key);
}

//!-------------------------------------------------------------------
bool MyWindow::ShouldClose() const
{
    return _mainWindow && glfwWindowShouldClose(_mainWindow);
}

//!-------------------------------------------------------------------
void MyWindow::SwapBuffer() const
{
    if( _mainWindow )
    {
        glfwSwapBuffers(_mainWindow);
    }
}

//!-------------------------------------------------------------------
void MyWindow::RegisterEventHandlers()
{
    glfwSetKeyCallback(_mainWindow, HandleKeyEvent);
    glfwSetCursorPosCallback(_mainWindow, HandleMouseEvent);
    //glfwSetScrollCallback(_mainWindow, HandleMouseScollEvent);
}

//!-------------------------------------------------------------------
void MyWindow::HandleKeyEvent( GLFWwindow* window, int key, int code,
                                                   int action, int mode )
{
    MyWindow* theWindow = static_cast<MyWindow*>(glfwGetWindowUserPointer(window));
    if( key == GLFW_KEY_ESCAPE && action == GLFW_PRESS )
    {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }

    if( key >= 0 && key < 1024 )
    {
        if( action == GLFW_PRESS )
        {
            theWindow->_keys.set(key, true);
            //std::cout << "Pressed : " << key << std::endl;
        }
        else if( action == GLFW_RELEASE )
        {
            theWindow->_keys.set(key, false);
            //std::cout << "Released : " << key << std::endl;
        }
        theWindow->_keyAcceptStates.set(key, false);
    }
}

//!-------------------------------------------------------------------
void MyWindow::HandleMouseEvent( GLFWwindow* window, double xPos, double yPos )
{
    MyWindow* theWindow = static_cast<MyWindow*>(glfwGetWindowUserPointer(window));

    if( theWindow->_firstMouseMove )
    {
        theWindow->_lastX = xPos;
        theWindow->_lastY = yPos;
        theWindow->_firstMouseMove = false;
    }

    theWindow->_xChange = xPos - theWindow->_lastX;
    theWindow->_yChange = theWindow->_lastY - yPos; // Considering up as +ve y-axis.

    theWindow->_lastX = xPos;
    theWindow->_lastY = yPos;

    // printf( "X : %.6f, Y : %.6f\n", theWindow->_xChange, theWindow->_yChange);
}

//!-------------------------------------------------------------------
void MyWindow::HandleMouseScollEvent( GLFWwindow* window, double xOffset, double yOffset )
{
    MyWindow* theWindow = static_cast<MyWindow*>(glfwGetWindowUserPointer(window));

    //if( theWindow->_fov >= 1.0f && theWindow->_fov <= 120.0f )
    //{
    //    theWindow->_fov -= yOffset;
    //}

    //if( theWindow->_fov < 1.0f )
    //{
    //    theWindow->_fov = 1.0f;
    //}
    //if( theWindow->_fov > 120.0f )
    //{
    //    theWindow->_fov = 120.0f;
    //}
    theWindow->_lastFov = theWindow->_fov;
    theWindow->_fov -= ((float)yOffset * 2.0f);
}

//!-------------------------------------------------------------------
GLfloat MyWindow::GetXChange()
{
    GLfloat theChange = _xChange;
    _xChange = 0.0f;
    return theChange;
}

//!-------------------------------------------------------------------
GLfloat MyWindow::GetYChange()
{
    GLfloat theChange = _yChange;
    _yChange = 0.0f;
    return theChange;
}

//!-------------------------------------------------------------------
GLfloat MyWindow::GetFOVChange()
{
    GLfloat change = (_fov - _lastFov);
    _lastFov = _fov;
    return change;
}

//!-------------------------------------------------------------------
glm::mat4 MyWindow::GetProjectionMatrix()
{
    return glm::perspective(glm::radians(60.0f), BufferWidth() / BufferHeight(), 0.1f, 100.0f );
}

//!-------------------------------------------------------------------
MyWindow::~MyWindow()
{
    if( _mainWindow )
    {
        glfwDestroyWindow(_mainWindow);
    }
    glfwTerminate();
}

