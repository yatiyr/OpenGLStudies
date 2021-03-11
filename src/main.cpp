#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <ShaderProgram.h>


// In order to draw something we need vertices.
// OpenGL only processes 3D coordinates when
// they are in a specific range between -1.0
// and 1.0 on all 3 axes. All coordinates within
// this so called called normalized device coordinates
// range will end up visible on our screen
float vertices[] = {
    -0.5f, -0.5f, 0.0f,
     0.5f, -0.5f, 0.0f,
     0.0f,  0.5f, 0.0f
};


float verticesRectangle[] = {
    0.5f,  0.5f, 0.0f, // top right
    0.5f, -0.5f, 0.0f, // bottom right
   -0.5f, -0.5f, 0.0f, // bottom left
   -0.5f,  0.5f, 0.0f, // top left
};

unsigned int indices[] = {
    0, 1, 3,
    1, 2, 3
};



// In order for OpenGL to use the shader it has to dynamically
// compile it at run-time from its source code.
const char* vertexShaderSource = "#version 330 core\n"
                                 "layout (location=0) in vec3 aPos;\n"
                                 "void main()\n"
                                 "{\n"
                                 "  gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
                                 "}\0";


// We declare fragment shader source just like vertex shader source
const char* fragmentShaderSource = "#version 330 core\n"
                                   "out vec4 FragColor;\n"
                                   "void main()\n"
                                   "{\n"
                                   "    FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);"
                                   "}\0";                               

// Callback function which is going to be called
// when we resize our window
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}


// For input control
void processInput(GLFWwindow* window) {

    // If escape key is not pressed then glfwGetKey returns
    // GLFW_RELEASE
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }
}

int main() {

    // We initialize OpenGL
    glfwInit();

    // We tell what version we want
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // We create a pointer to a GLFWindow object and if we 
    // fail creating it, terminate OpenGL
    GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGLStudies", nullptr, nullptr);
    if(window == nullptr)
    {
        std::cout << "Failed to create GLFW window" << '\n';
        glfwTerminate();
        return -1;
    }


    // We change the current context into our window that we've created
    glfwMakeContextCurrent(window);

    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << '\n';
        return -1;
    }

    // We initialize viewport
    glViewport(0, 0, 800, 600);


    // Specify clear color
    // This is a state setting function
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);


    // We register a callback function for detecting
    // window size change events in order to change
    // viewport accordingly
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);


    // Initialize shader program from sources
    ShaderProgram program(vertexShaderSource,fragmentShaderSource);
    program.useProgram();

    // create vertex array object
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // Initialize vertex buffer object to send vertex data to GPU at once
    unsigned int VBO;
    glGenBuffers(1, &VBO);

    // We bind VBO to GL_ARRAY_BUFFER because it sends vertex data
    // from now on, every buffer calls we make on GL_ARRAY_BUFFER
    // will be used to configure VBO.
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // We call glBufferData to copy previously defined vertex data
    // into buffer's memory
    // GL_STATIC_DRAW : the data is set only once and used many times
    // GL_STREAM_DRAW : the data is set only once and used by the GPU at most a few times
    // GL_DYNAMIC_DRAW : the data is changed a lot and used many times
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesRectangle), verticesRectangle, GL_STATIC_DRAW);

    // create EBO to store indices
    unsigned int EBO;
    glGenBuffers(1, &EBO);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // glVertexAttribPointer tells OpenGL how to interpret the vertex data (per vertex attribute)
    // param 1 -> which vertex attribute we want to configure. In the shader we specified
    //            layout (location = 0). This sets the location of the vertex attribute to 0
    //            and since we want to pass data to this vertex attribute we pass in 0
    //
    // param 2 -> size of the vertex attribute. Vertex attribute is vec3 so we have 3 values
    // param 3 -> type of the data which is GL_FLOAT
    // param 4 -> if we want normalized data (between -1,0,1)
    // param 5 -> tells us the space between consecutive vertex attributes (stride)
    // param 6 -> this is the offset of where the position data begins in the buffer
    //            since the position data is at the start  of the data array this value
    //            is just 0.
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);


    glBindVertexArray(0);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    // Our render loop, keeps on running untel we tell
    // GLFW to stop.
    // glfwWindowShouldClose: checks at the start of
    //                        each loop iteration if
    //                        GLFW has been instructed
    //                        to close.
    while(!glfwWindowShouldClose(window))
    {
        // input
        processInput(window);

        // This is a state-using function
        // it clears the buffer with clear
        // color we specify
        glClear(GL_COLOR_BUFFER_BIT);

        program.useProgram();
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        // check and call events and callback functions
        // if we defined them
        glfwPollEvents();

        // swap the color buffer (a large 2D buffer that
        //                        contains color values for 
        //                        each pixel in GLFW's window) 
        glfwSwapBuffers(window);


        // ------------- DOUBLE BUFFER -------------- //
        /*
         When an application draw in a single buffer,
         the resulting image may  display   flickery 
         issues. This is because the resulting output
         image is not drawn in an instant, but drawn
         pixel by pixel and usually from left to right
         and top to bottom. Because this image is not
         displayed at an instant to the user while still
         being rendered to, the result may contain
         artifacts. To circumvent these issues, windowing
         applications apply a double buffer for rendering.
         The front buffer contains the final output image
         that is shown at the screen, while all the 
         rendering commands draw to the back buffer.
         As soon as all the rendering commands are
         finished we swap the back buffer to the front
         buffer so the image can be displayed without
         still being rendered to, removing all the
         aforomentioned artifacts.
                                                      */
        // ------------------------------------------ //


    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    // To exit gracefully, we clean all of GLFW's
    // resources that were allocated.
    glfwTerminate();
    return 0;
}