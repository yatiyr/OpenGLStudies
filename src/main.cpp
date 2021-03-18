#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <math.h>
#include <stb_image.h>

#include <algorithm>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <ShaderProgram.h>
#include <Camera.h>
#include <GeometryData.h>
#include <Utils.h>

#include <iostream>

#include <Model.h>
#include <RootDir.h>


// GLOBAL VARIABLES FOR DELTATIME
float deltaTime = 0.0f; // Time between current and the last frame
float lastFrame = 0.0f; // Time of last frame

// window settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// Camera settings
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;


// Callback function which is going to be called
// when we resize our window
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {

    // make sure the viewport matches the new window dimensions
    // width and height will be significantly larger than specified
    // on retina displays
    glViewport(0, 0, width, height);
}

// Callback function for taking inputs from mouse
void mouse_callback(GLFWwindow* window, double xpos, double ypos) {

    if(firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed because y coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}

// For input control
void processInput(GLFWwindow* window) {

    // If escape key is not pressed then glfwGetKey returns
    // GLFW_RELEASE
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }

    // camera inputs
    if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

glm::vec3 lightPos(5.0f, 3.0f, -3.0f);

int main() {

    // We initialize OpenGL
    glfwInit();

    // We tell what version we want
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

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
    // We register a callback function for detecting
    // window size change events in order to change
    // viewport accordingly
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // Register callback for taking inputs from mouse
    glfwSetCursorPosCallback(window, mouse_callback);

    // Register scroll callbacks
    glfwSetScrollCallback(window, scroll_callback);

    // Capture and hide mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);    

    // tell glad load all opengl function pointers
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << '\n';
        return -1;
    }

    // global opengl state -> depth test
    glEnable(GL_DEPTH_TEST);

    // Initialize shader program from sources
    ShaderProgram program("shaders/modelShader/model_shader.vert","shaders/modelShader/model_shader.frag");
    ShaderProgram *lightProgram = new ShaderProgram("shaders/lightShader/shader_light.vert", "shaders/lightShader/shader_light.frag");



    // ------------------- FOR CREATING LIGHT BUFFERS ------------------------ //
    unsigned int lightVAO;
    glGenVertexArrays(1, &lightVAO);
    glBindVertexArray(lightVAO);

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
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesDiffuseMap), verticesDiffuseMap, GL_STATIC_DRAW);

    // we can use container's VBO. It has the data already
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // set vertex attribPointer
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
    // ----------------------------------------------------------------------- //
    program.use();
    program.setFloat("material.shininess", 32.0f);    
    // ------------------ SET TEXTURE ------------------- //

    // flip loaded textures vertically
    setVerticalFlip(true);
    std::string pathBackpack = ROOT_DIR + std::string("assets/models/backpack/backpack.obj");

    Model m(pathBackpack.c_str());



 
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    // Our render loop, keeps on running until we tell
    // GLFW to stop.
    // glfwWindowShouldClose: checks at the start of
    //                        each loop iteration if
    //                        GLFW has been instructed
    //                        to close.
    while(!glfwWindowShouldClose(window))
    {

        // Calculate deltatime
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        processInput(window);

    // ------------------- RENDERING CODE --------------------- //

        // Specify clear color
        // This is a state setting function
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);        
        // This is a state-using function
        // it clears the buffer with clear
        // color we specify
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // activate our shader program
        program.use();

        // send time uniform for animation
        program.setFloat("time", currentFrame);

        // Send light position to shader program
        lightPos.x = sin(glfwGetTime() * 1) * 3.0f;
        lightPos.z = cos(glfwGetTime() * 1) * 3.0f;
        lightPos.y = 0.0f;
        // give light properties to shader
        glm::vec3 lightColor;
        lightColor.x = 1.0f;  //std::max(sin(glfwGetTime() * 2.0f), 0.7);
        lightColor.y = 1.0f;  //std::max(sin(glfwGetTime() * 0.7f), 0.6);
        lightColor.z = 1.0f;  //std::max(sin(glfwGetTime() * 1.3f), 0.9);

        glm::vec3 diffuseColor = lightColor * glm::vec3(0.2f);
        glm::vec3 ambientColor = diffuseColor * glm::vec3(0.1f);


        // Directional light
        program.setVec3("directionalLight.direction", -0.2f, -1.0f, -0.3f);        
        program.setVec3("directionalLight.ambient", ambientColor);
        program.setVec3("directionalLight.diffuse", diffuseColor);
        program.setVec3("directionalLight.specular", 1.0f, 1.0f, 1.0f);        


        // Point lights
        for(int i=0; i<4; i++)
        {
            glm::vec3 diffuseColor = pointLightColors[i] * glm::vec3(0.7);
            glm::vec3 ambientColor = pointLightColors[i] * glm::vec3(0.5f);

            program.setVec3("pointLights[" + std::to_string(i) + "].position", pointLightPositions[i]);
            program.setVec3("pointLights[" + std::to_string(i) + "].ambient", ambientColor);
            program.setVec3("pointLights[" + std::to_string(i) + "].diffuse", diffuseColor);
            program.setVec3("pointLights[" + std::to_string(i) + "].specular", 1.0f, 1.0f, 1.0f);

            program.setFloat("pointLights[" + std::to_string(i) + "].constant", 1.0f);
            program.setFloat("pointLights[" + std::to_string(i) + "].linear", 0.2f);
            program.setFloat("pointLights[" + std::to_string(i) + "].quadratic", 0.302f);             

        }


        // Spot Light
        program.setVec3("spotLight.position", camera.Position);
        program.setVec3("spotLight.direction", camera.Front);
        program.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
        program.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(17.5f)));

        program.setVec3("spotLight.ambient", ambientColor);
        program.setVec3("spotLight.diffuse", diffuseColor);
        program.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);

        program.setFloat("spotLight.constant", 1.0f);
        program.setFloat("spotLight.linear", 0.03f);
        program.setFloat("spotLight.quadratic", 0.012f);             

        // Send view position to shader program
        program.setVec3("viewPos", camera.Position);

        // pass projection matrix to shader -> changes every frame because of camera
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float) SCR_HEIGHT, 0.1f, 100.0f);
        program.set4Matrix("projection", projection);

        // camera/view transformation
        glm::mat4 view = camera.GetViewMatrix();
        program.set4Matrix("view", view);

        glm::mat4 model(1.0f);
        model = glm::translate(model, glm::vec3(-1.0, 2.0f,-3.0f));
        model = glm::scale(model, glm::vec3(1.f,1.f,1.f));

        glm::mat3 normalMatrix = glm::transpose(glm::inverse(model));

        program.set4Matrix("model", model);
        program.set3Matrix("normalMatrix", normalMatrix);
        
        m.Draw(program);

        glBindVertexArray(lightVAO);
        lightProgram->use();
        lightProgram->set4Matrix("projection", projection);
        lightProgram->set4Matrix("view", view);

        for(int i=0; i<4; i++)
        {
            glm::mat4 modelLight = glm::mat4(1.0f);
            modelLight = glm::translate(modelLight, pointLightPositions[i]);
            modelLight = glm::scale(modelLight, glm::vec3(0.1f));

            lightProgram->set4Matrix("model", modelLight);
            lightProgram->setVec3("uColor", pointLightColors[i]);
            glDrawArrays(GL_TRIANGLES, 0, 36);            
        }
                

        // swap the color buffer (a large 2D buffer that
        //                        contains color values for 
        //                        each pixel in GLFW's window) 
        glfwSwapBuffers(window);

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

    glDeleteVertexArrays(1, &lightVAO);
    glDeleteBuffers(1, &VBO);
    delete lightProgram;
    // To exit gracefully, we clean all of GLFW's
    // resources that were allocated.
    glfwTerminate();
    return 0;
}
