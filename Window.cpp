#include "Window.h"

Window::Window()
{
    width = 800;
    height = 600;

    for (size_t i = 0; i < 1024; i++)
    {
        keys[i] = false;
    }

    bufferHeight = -1;
    bufferWidth = -1;
    mainWindow = NULL;
    xChange = 0.0f;
    yChange = 0.0f;
    lastX = 0.0f;
    lastY = 0.0f;
    mouseFirstMoved = true;
}

Window::Window(GLint windowWidth, GLint windowHeight) : Window()
{
    width = windowWidth;
    height = windowHeight;
}

int Window::initialise()
{
    //initialise GLFW
    if (!glfwInit()) {
        printf("GLFW initialisation failed!\n");
        glfwTerminate();
        return 1;
    }

    // Setup GLFW window properties
    // OpenGL version 
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    // Core profile - no backwards compatibility
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // Allow forward compatibility
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    mainWindow = glfwCreateWindow(width, height, "test_window", NULL, NULL);
    if (!mainWindow)
    {
        printf("GLFW window creation failed! \n");
        glfwTerminate();
        return 1;
    }

    // Get buffer size info (middle part of the viewport)
    glfwGetFramebufferSize(mainWindow, &bufferWidth, &bufferHeight);

    // Set context for GLEW to use, use the window we just created and draw onto that
    glfwMakeContextCurrent(mainWindow);

    //handle the key+ mouse input
    createCallbacks();
    // cursor no longer visible
    glfwSetInputMode(mainWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Alow modern extensions of opengl
    glewExperimental = GL_TRUE;

    if (glewInit() != GLEW_OK)
    {
        printf("GLEW initialisation failed!");
        glfwDestroyWindow(mainWindow); //destroy the already created window
        glfwTerminate();
        return 1;
    }

    // enable the depth test
    glEnable(GL_DEPTH_TEST);

    // Setup Viewport size, make the entire window our viewport
    glViewport(0, 0, bufferWidth, bufferHeight);

    //set the owner so GLFW knows the class
    glfwSetWindowUserPointer(mainWindow, this);

    // get version info
    const GLubyte* renderer = glGetString(GL_RENDERER); // get renderer string
    const GLubyte* version = glGetString(GL_VERSION); // version as a string
    printf("Renderer: %s\n", renderer);
    printf("OpenGL version supported %s\n", version);

    return 0;
}

void Window::createCallbacks()
{
    // when a key is pressed, use the required callback function
    glfwSetKeyCallback(mainWindow, handleKeys);
    glfwSetCursorPosCallback(mainWindow, handleMouse);
}

GLfloat Window::getXChange()
{
    GLfloat theChange = xChange;
    xChange = 0.0f;
    return theChange;
}
GLfloat  Window::getYChange()
{
    GLfloat theChange = yChange;
    yChange = 0.0f;
    return theChange;
}

void Window::handleKeys(GLFWwindow* window, int key, int code, int action, int mode)
{
    //get the object pointer 
    Window* theWindow = static_cast<Window*> (glfwGetWindowUserPointer(window));

    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }

    //make sure the key is in valid range
    if (key >= 0 && key < 1024)
    {
        // set true or false if the key has been pressed
        if (action == GLFW_PRESS)
        {
            theWindow->keys[key] = true;
            ///printf("pressed %d \n", key);
        }
        else if (action == GLFW_RELEASE)
        {
            theWindow->keys[key] = false;
            ///printf("released %d \n", key);
        }
    }
}

void Window::handleMouse(GLFWwindow* window, double xPos, double yPos)
{
    //get the object pointer 
    Window* theWindow = static_cast<Window*> (glfwGetWindowUserPointer(window));
    if (theWindow->mouseFirstMoved)
    {
        theWindow->lastX = xPos;
        theWindow->lastY = yPos;
        theWindow->mouseFirstMoved = false;
    }
    theWindow->xChange = xPos - theWindow->lastX;
    theWindow->yChange =  theWindow->lastY -  yPos; //reverse inversion

    theWindow->lastX = xPos;
    theWindow->lastY = yPos;

}
Window::~Window()
{
    glfwDestroyWindow(mainWindow); //destroy the already created window
    glfwTerminate();
}