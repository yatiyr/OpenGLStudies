#include <Renderer.h>

#include <Lessons/Ubo/LessonUbo.h>

void Renderer::RegisterCallbacks()
{

    // We register a callback function for detecting window size
    // change events in order to change viewport accordingly
    glfwSetFramebufferSizeCallback(_window, framebuffer_size_callback);

    // Register callback for taking inputs from mouse
    glfwSetCursorPosCallback(_window, mouse_callback);

    // Register scroll callbacks
    glfwSetScrollCallback(_window, scroll_callback);

    // Capture and hide mouse
    glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void Renderer::ProcessInput()
{
    // If escape key is pressed then glfwGetKey returns
    // GLFW_RELEASE
    if(glfwGetKey(_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(_window, true);
    }

    // Camera Inputs
    if(glfwGetKey(_window, GLFW_KEY_W) == GLFW_PRESS)
    {
        activeCamera->ProcessKeyboard(FORWARD, deltaTime);
    }
    if(glfwGetKey(_window, GLFW_KEY_S) == GLFW_PRESS)
    {
        activeCamera->ProcessKeyboard(BACKWARD, deltaTime);
    }
    if(glfwGetKey(_window, GLFW_KEY_A) == GLFW_PRESS)
    {
        activeCamera->ProcessKeyboard(LEFT, deltaTime);
    }
    if(glfwGetKey(_window, GLFW_KEY_D) == GLFW_PRESS)
    {
        activeCamera->ProcessKeyboard(RIGHT, deltaTime);
    }
}

Renderer::Renderer()
{

    Camera *cam = new Camera(glm::vec3(5.0f, 0.0f, 3.0f));
    _cameras.push_back(cam);
    activeCamera = cam;

    // We initialize OpenGL
    glfwInit();

    // We tell what version we want
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);    
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    _window = glfwCreateWindow(800, 600, "OpenGLStudies", nullptr, nullptr);
    if(_window == nullptr)
    {
        glfwTerminate();
        throw std::runtime_error("Failed to create GLFW window");
    }

    // We change the current context into our window that we've created
    glfwMakeContextCurrent(_window);

    // Callback functions are registered
    RegisterCallbacks();

    // Tell GLAD load all OpenGL function pointers
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        throw std::runtime_error("Failed to initialize GLAD");
    }

    // Put here initialConf() function of specific lesson class
    LessonUbo::initialConf();
}

void Renderer::Run() 
{



    // Our render loop, keeps on running until we tell
    // GLFW to stop
    // glfwWindowShouldClose : checks at the start of
    //                         each loop iteration if
    //                         GLFW has been instructed
    //                         to close.
    while(!glfwWindowShouldClose(_window))
    {
        // Calculate deltatime
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        ProcessInput();

        // Put here draw() function of lesson class
        LessonUbo::draw(activeCamera);

        // swap the color buffer (a large 2D buffer that
        //                        contains color values
        //                        for each pixel in GLFW's
        //                        window)
        glfwSwapBuffers(_window) ;

        // check and call events and callback functions
        // if we defined them
        glfwPollEvents();

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
    // glDeleteVertexArrays(1, &lightVAO);
    // glDeleteBuffers(1, &VBO);
    // glDeleteProgram(_shaderPrograms[0].id);
    // glDeleteProgram(_shaderPrograms[1].id);

    // Put here clean function of lesson class
    LessonUbo::clean();

    // To exit gracefully, we clean all of GLFW's
    // resources that were allocated
    glfwTerminate();

}

Renderer::~Renderer()
{

}
