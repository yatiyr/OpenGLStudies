#ifndef __RENDERER_H__
#define __RENDERER_H__

#include <math.h>
#include <vector>
#include <iostream>
#include <algorithm>

// ---- OPENGL and GLFW ----- //
#include <glad/glad.h>
#include <GLFW/glfw3.h>
// ---------- GLM ----------- //
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
// ---------- LOCAL --------- //
#include <Utils.h>
#include <Model.h>
#include <Camera.h>
#include <RootDir.h>
#include <ShaderProgram.h>
#include <GeometryData.h>
// ----------- STB ---------- //
#include <stb_image.h>
// -------------------------- //

#include <GLFWCallbacks.h>

extern Camera *activeCamera;

class Renderer {

private:

// ----------------------- WINDOW VARIABLE --------------------- //

    GLFWwindow *_window;

// -------------------- VARIABLES FOR DELTATIME ---------------- //

    // Time between current and the last frame
    float deltaTime = 0.0f;

    // Time of last frame
    float lastFrame = 0.0f;

// -------------------- WINDOW SETTING VARIABLES --------------- //

    const unsigned int SRC_WIDTH  = 800;
    const unsigned int SRC_HEIGHT = 600; 

// ------------------------ CAMERA SETTINGS -------------------- //

    std::vector<Camera*> _cameras;

// ------------------ CALLBACK REGISTER FUNCTION --------------- //

    void RegisterCallbacks();

// -------------------- INPUT PROCESS FUNCTION ----------------- //

    void ProcessInput();

// ------------------------ LIGHT SETTINGS --------------------- //

    // TODO: MAKE LIGHTS A VECTOR TOO
    glm::vec3 lightPosition;

// --------------------------- SHADERS ------------------------- //

    void LoadShaders();
    std::vector<ShaderProgram> _shaderPrograms;

// ------------------------------------------------------------- //

public:

    Renderer();
    ~Renderer();

    void Run();

};



#endif /* __RENDERER_H__ */