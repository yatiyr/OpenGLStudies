#include <GLFWCallbacks.h>
#include <iostream>

Camera *activeCamera;

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{

    // make sure the viewport matches the new window dimension's
    // width and height will be significantly larger than specified
    // on retina displays
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xPos, double yPos)
{

    if(activeCamera->firstMouse)
    {
        activeCamera->lastX = xPos;
        activeCamera->lastY = yPos;
        activeCamera->firstMouse = false;
    } 

    float xOffset = xPos - activeCamera->lastX;
    // It is reversed because y coordinates go
    // from bottom to top
    float yOffset = activeCamera->lastY - yPos;

    activeCamera->lastX = xPos;
    activeCamera->lastY = yPos;

    activeCamera->ProcessMouseMovement(xOffset, yOffset);
    
}

void scroll_callback(GLFWwindow* window, double xOffset, double yOffset)
{
    activeCamera->ProcessMouseScroll(yOffset);
}