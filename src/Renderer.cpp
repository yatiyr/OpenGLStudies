#include <Renderer.h>

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

void Renderer::LoadShaders()
{
    ShaderProgram program("shaders/modelShader/model_shader.vert",
                          "shaders/modelShader/model_shader.frag");

    ShaderProgram lightProgram("shaders/lightShader/shader_light.vert",
                               "shaders/lightShader/shader_light.frag");

    _shaderPrograms.push_back(std::move(program));
    _shaderPrograms.push_back(std::move(lightProgram));
}

void Renderer::LoadModels()
{
    std::string backpackPath = ROOT_DIR + std::string("assets/models/backpack/backpack.obj");
    Model backpackModel(backpackPath.c_str());

    std::string sponzaPath = ROOT_DIR + std::string("assets/models/sponza/Sponza.gltf");
    Model sponzaModel(sponzaPath.c_str());

    _models.push_back(std::move(backpackModel));
    _models.push_back(std::move(sponzaModel));
}

void Renderer::WriteLightsToShader(ShaderProgram& shader)
{
    // give light properties to shader
    glm::vec3 lightColor;
    lightColor.x = 1.0f;
    lightColor.y = 1.0f;
    lightColor.z = 1.0f;

    glm::vec3 diffuseColor = lightColor * glm::vec3(0.2f);
    glm::vec3 ambientColor = diffuseColor * glm::vec3(0.1f);    

    // Directional light
    shader.setVec3("directionalLight.direction", -0.2f, -1.0f, -0.3f);
    shader.setVec3("directionalLight.ambient", ambientColor);
    shader.setVec3("directionalLight.diffuse", diffuseColor);
    shader.setVec3("directionalLight.specular", 1.0f, 1.0f, 1.0f);

    for(int i=0; i<4; i++)
    {
        glm::vec3 diffuseColor = pointLightColors[i] * glm::vec3(0.7);
        glm::vec3 ambientColor = pointLightColors[i] * glm::vec3(0.5f);

        shader.setVec3("pointLights[" + std::to_string(i) + "].position", pointLightPositions[i]);
        shader.setVec3("pointLights[" + std::to_string(i) + "].ambient", ambientColor);
        shader.setVec3("pointLights[" + std::to_string(i) + "].diffuse", diffuseColor);
        shader.setVec3("pointLights[" + std::to_string(i) + "].specular", 1.0f, 1.0f, 1.0f);

        shader.setFloat("pointLights[" + std::to_string(i) + "].constant", 1.0f);
        shader.setFloat("pointLights[" + std::to_string(i) + "].linear", 0.2f);
        shader.setFloat("pointLights[" + std::to_string(i) + "].quadratic", 0.302f);              
    }

    // Spot Light
    shader.setVec3("spotLight.position", activeCamera->Position);
    shader.setVec3("spotLight.direction", activeCamera->Front);
    shader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
    shader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(17.5f)));

    shader.setVec3("spotLight.ambient", ambientColor);
    shader.setVec3("spotLight.diffuse", diffuseColor);
    shader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);

    shader.setFloat("spotLight.constant", 1.0f);
    shader.setFloat("spotLight.linear", 0.03f);
    shader.setFloat("spotLight.quadratic", 0.012f);
}

void Renderer::DrawModels(ShaderProgram& shader)
{

    // TODO: Turn this into a loop later
    shader.use();    

    WriteLightsToShader(shader);

    // Send view position to shader program
    shader.setVec3("viewPos", activeCamera->Position);

    // Pass projection matrix to shader -> changes every frame because of camera
    glm::mat4 projection = glm::perspective(glm::radians(activeCamera->Zoom), (float) SRC_WIDTH / (float) SRC_HEIGHT, 0.1f, 10000.0f);
    shader.set4Matrix("projection", projection);

    // camera/view transformation
    glm::mat4 view = activeCamera->GetViewMatrix();
    shader.set4Matrix("view", view);

    glm::mat4 model(1.0f);
    model = glm::translate(model, glm::vec3(-1.0f, 2.0f, -3.0f));
    model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));

    glm::mat3 normalMatrix = glm::transpose(glm::inverse(model));

    shader.set4Matrix("model", model);
    shader.set3Matrix("normalMatrix", normalMatrix);


    _models[0].Draw(shader);

    glm::mat4 model2(1.0f);
    model2 = glm::translate(model2, glm::vec3(0.0f, -2.0f, 0.0f));    
    model2 = glm::scale(model2, glm::vec3(0.1f, 0.1f, 0.1f));    
    shader.set4Matrix("model", model2);    
    _models[1].Draw(shader);

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

    // Global OpenGL state -> depth test
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    // Initialize shader programs from sources
    LoadShaders();
}

void Renderer::Run() 
{

    // --------------- FOR CREATING LIGHT BUFFERS ------------------ //
    unsigned int lightVAO;
    glGenVertexArrays(1, &lightVAO);
    glBindVertexArray(lightVAO);

    unsigned int VBO;
    glGenBuffers(1, &VBO);

    // We bind VBO to GL_ARRAY_BUFFER because it sends vertex data
    // from now on, every buffer calls we make on GL_ARRAY_BUFFER
    // will be used to configure VBO
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // We call glBufferData to copy previously defined vertex data
    // into buffer's memory
    // GL_STATIC_DRAW : the data is set only once and used many times
    // GL_STREAM_DRAW : the data is set only once and used by the GPU
    //                  at most a few times
    // GL_DYNAMIC_DRAW : the date is changed a lot and used many times
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesDiffuseMap), verticesDiffuseMap, GL_STATIC_DRAW);

    // we can use container's VBO. It has the data already
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // set vertex attribPointer
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);

    _shaderPrograms[0].use();
    _shaderPrograms[0].setFloat("material.shininess", 32.0f);

    // flip loaded textures vertically
    setVerticalFlip(true);


    LoadModels();

    
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

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

    // ------ RENDERING CODE ------ //

        // Specify clear color
        // This is a state setting function
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

        // This is a state-using function
        // it clears the buffer with clear
        // color we specify
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // activate our shader program
        _shaderPrograms[0].use();

        // send time uniform for animation
        _shaderPrograms[0].setFloat("time", currentFrame);

        // Send light position to shader program
        lightPosition.x = sin(glfwGetTime() * 1) * 3.0f;
        lightPosition.z = cos(glfwGetTime() * 1) * 3.0f;
        lightPosition.y = 0.0f;

        // Pass projection matrix to shader -> changes every frame because of camera
        glm::mat4 projection = glm::perspective(glm::radians(activeCamera->Zoom), (float) SRC_WIDTH / (float) SRC_HEIGHT, 0.1f, 100.0f);
        _shaderPrograms[0].set4Matrix("projection", projection);

        // camera/view transformation
        glm::mat4 view = activeCamera->GetViewMatrix();
        _shaderPrograms[0].set4Matrix("view", view);

        DrawModels(_shaderPrograms[0]);


        glBindVertexArray(lightVAO);
        _shaderPrograms[1].use();
        _shaderPrograms[1].set4Matrix("projection", projection);
        _shaderPrograms[1].set4Matrix("view", view);   

        for(int i=0; i<4; i++)
        {
            glm::mat4 modelLight = glm::mat4(1.0f);
            modelLight = glm::translate(modelLight, pointLightPositions[i]);
            modelLight = glm::scale(modelLight, glm::vec3(0.1f));

            _shaderPrograms[1].set4Matrix("model", modelLight);
            _shaderPrograms[1].setVec3("uColor", pointLightColors[i]);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }


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
    glDeleteVertexArrays(1, &lightVAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(_shaderPrograms[0].id);
    glDeleteProgram(_shaderPrograms[1].id);

    // To exit gracefully, we clean all of GLFW's
    // resources that were allocated
    glfwTerminate();

}

Renderer::~Renderer()
{

}
