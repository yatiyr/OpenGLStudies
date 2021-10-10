#ifndef __GLFW_CALLBACKS_H__
#define __GLFW_CALLBACKS_H__

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <Camera.h>

/* 
     --------------------------------------------------------
    | Callback functions for  glfw windows are declared here | 
     --------------------------------------------------------
                                                                */


/**
 * Callback function which is going to be called when we
 * resize our window
 */
void framebuffer_size_callback(GLFWwindow* window, int width, int height);

/**
 * Callback function for taking inputs from mouse
 */
void mouse_callback(GLFWwindow* window, double xPos, double yPos);

/**
 * Callback function for mouse scroll
 */
void scroll_callback(GLFWwindow* window, double xOffset, double yOffset);



















#endif /* __GLFW_CALLBACKS_H__ */