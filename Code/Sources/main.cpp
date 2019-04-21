#include "Physics/physicsengine.hpp"
#include "Rendering/renderingengine.hpp"
#include "Rendering/cubemap.hpp"
#include "Assets/shader.hpp"
#include "Core/scene.hpp"
#include "Objects/car.hpp"
#include "Objects/terrain.hpp"
#include "Objects/wall.hpp"
#include "Objects/streetlight.hpp"
#include "globals.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <cstdio>
#include <cstdlib>
#include <iomanip>
#include <memory>
#include <iostream>
#include <sstream>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

const int FPS_ROLLING_FRAMES = 10;
const int NUM_STREETLIGHTS = 24;
const float STREETLIGHT_OFFSET = 0.5f;
const float TRACK_INNER_A = 58.0f;
const float TRACK_INNER_B = 58.0f;
const float TRACK_OUTER_A = 63.0f;
const float TRACK_OUTER_B = 63.0f;

// The following globals are used in the GLFW callbacks
Core::Scene scene;
bool firstMouse = true;
double lastX;
double lastY;

// FPS determination
double determineFPS(double deltaTime)
{
    static double rollingFPS;
    static double latestFrameDeltas[FPS_ROLLING_FRAMES];
    static int latestFrameIdx = 0;
    static bool firstPassFrames = true;

    latestFrameDeltas[latestFrameIdx] = deltaTime;
    if (firstPassFrames) {
        float deltas = 0;
        for (int i = 0; i <= latestFrameIdx; i++) {
            deltas += latestFrameDeltas[i];
        }
        rollingFPS = (latestFrameIdx+1)/deltas;
    }
    else {
        float deltas = 0;
        for (int i = 0; i < FPS_ROLLING_FRAMES; i++) {
            deltas += latestFrameDeltas[i];
        }
        rollingFPS = FPS_ROLLING_FRAMES/deltas;
    }
    latestFrameIdx = ++latestFrameIdx % FPS_ROLLING_FRAMES;
    if (latestFrameIdx == 0) {
        firstPassFrames = false;
    }

    return rollingFPS;
}


// Callbacks
void errorCallback(int error, const char* description)
{
    std::cout << "Error: " << description << std::endl;
}

void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    // Process-once key events should happen here
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
    else if (key == GLFW_KEY_M && action == GLFW_PRESS) {
        if (scene.mRenderSettings.mRenderMode == Rendering::RenderMode::DEBUG) {
            scene.mRenderSettings.mRenderMode = Rendering::RenderMode::DEFERRED_SHADING;
        }
        else {
            scene.mRenderSettings.mRenderMode = Rendering::RenderMode::DEBUG;
        }
    }
    else if (key == GLFW_KEY_T && action == GLFW_PRESS) {
        scene.mRenderSettings.mDrawDebugLines = !scene.mRenderSettings.mDrawDebugLines;
    }
}


void mouseCallback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
        return;
    }

    double xoffset = xpos - lastX;
    // Reversed since y-coordinates range from bottom to top
    double yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;
}

void framebufferSizeCallback(GLFWwindow* window, int newWidth, int newHeight)
{
    scene.mRenderSettings.mFramebufferWidth = newWidth;
    scene.mRenderSettings.mFramebufferHeight = newHeight;
}

int main(int argc, char * argv[])
{   
    //******* PERFORM INITIALIZATION *******
    // Load GLFW
    glfwInit();

    // Register error callback
    glfwSetErrorCallback(errorCallback);

    // Create a window
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
    auto window = glfwCreateWindow(INIT_WIDTH, INIT_HEIGHT, "Driving Scene - Jeffrey Everett", nullptr, nullptr);

    // Determine framebuffer size
    int fbWidth, fbHeight;
    glfwGetFramebufferSize(window, &fbWidth, &fbHeight);

    // Check for valid context
    if (window == nullptr) {
        std::cout << "Failed to create OpenGL context. Exiting." << std::endl;
        exit(1);
    }

    // Create Context and load OpenGL Functions
    glfwMakeContextCurrent(window);
    gladLoadGL();
    std::cout << "Using OpenGL " << glGetString(GL_VERSION) << std::endl;

    // Initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    //******* CREATE ENGINES ******
    // Create base engines
    Physics::PhysicsEngine physicsEngine;
    Rendering::RenderingEngine renderingEngine(fbWidth, fbHeight);

    // Make necessary connections
    physicsEngine.connectDebugRenderer(&(*renderingEngine.mDebugRenderer));
    
    
    //******* CREATE REUSABLE ASSETS *******
    auto defaultGeometryShader = std::make_shared<Assets::Shader>(
        PROJECT_SOURCE_DIR "/Shaders/VertexShaders/gbuffer.vert",
        PROJECT_SOURCE_DIR "/Shaders/FragmentShaders/gbuffer.frag"
    );
    auto defaultTerrainShader = std::make_shared<Assets::Shader>(
        PROJECT_SOURCE_DIR "/Shaders/VertexShaders/terrain.vert",
        PROJECT_SOURCE_DIR "/Shaders/TessCtrlShaders/terrain.tcs",
        PROJECT_SOURCE_DIR "/Shaders/TessEvalShaders/terrain.tes",
        PROJECT_SOURCE_DIR "/Shaders/GeometryShaders/terrain.geom",
        PROJECT_SOURCE_DIR "/Shaders/FragmentShaders/terrain.frag"
    );

    //******* CREATE SCENE *******
    std::vector<std::string> darkFaces {
        PROJECT_SOURCE_DIR "/Textures/CubeMaps/DarkStormy/DarkStormyLeft2048.png",
        PROJECT_SOURCE_DIR "/Textures/CubeMaps/DarkStormy/DarkStormyRight2048.png",
        PROJECT_SOURCE_DIR "/Textures/CubeMaps/DarkStormy/DarkStormyUp2048.png",
        PROJECT_SOURCE_DIR "/Textures/CubeMaps/DarkStormy/DarkStormyDown2048.png",
        PROJECT_SOURCE_DIR "/Textures/CubeMaps/DarkStormy/DarkStormyFront2048.png",
        PROJECT_SOURCE_DIR "/Textures/CubeMaps/DarkStormy/DarkStormyBack2048.png"
    };
    scene.mCubeMap.setFaces(darkFaces);
    scene.mRenderSettings.mRenderMode = Rendering::RenderMode::DEFERRED_SHADING;
    scene.mRenderSettings.mDrawDebugLines = true;
    scene.mRenderSettings.mFramebufferWidth = fbWidth;
    scene.mRenderSettings.mFramebufferHeight = fbHeight;

    //******* CREATE GAMEOBJECTS *******
    // Setup objects
    Objects::Wall::setup();
    Objects::Car::setup(defaultGeometryShader);
    Objects::Terrain::setup(defaultTerrainShader);
    Objects::Streetlight::setup(defaultGeometryShader);

    // Add car
    glm::vec3 carStartingPosition = glm::vec3(-TRACK_INNER_A - (TRACK_OUTER_A-TRACK_INNER_A)/2, 1, 0);
    auto car = std::make_shared<Objects::Car>(glm::vec3(0), physicsEngine);
    scene.add(car);

    // Add streetlights
    for (int i = 0; i < NUM_STREETLIGHTS/2; i++) {
        float theta = glm::radians(360.0f)*i/(NUM_STREETLIGHTS/2);
        glm::vec3 posStreetlight1 = glm::vec3((TRACK_INNER_A-STREETLIGHT_OFFSET)*glm::cos(theta), 0, (TRACK_INNER_B-STREETLIGHT_OFFSET)*glm::sin(theta));
        glm::vec3 posStreetlight2 = glm::vec3((TRACK_OUTER_A+STREETLIGHT_OFFSET)*glm::cos(theta), 0, (TRACK_OUTER_B+STREETLIGHT_OFFSET)*glm::sin(theta));

        auto streetlight1 = std::make_shared<Objects::Streetlight>(posStreetlight1, -theta+glm::radians(180.0f), false);
        scene.add(streetlight1);

        auto streetlight2 = std::make_shared<Objects::Streetlight>(posStreetlight2, -theta+glm::radians(180.0f), true);
        scene.add(streetlight2);
    }
    auto streetlight = std::make_shared<Objects::Streetlight>(glm::vec3(0), 0, 0);
    scene.add(streetlight);

    // Add terrain
    auto terrain = std::make_shared<Objects::Terrain>(glm::vec3(0), physicsEngine);
    //scene.add(terrain);

    //******* Register remaining callbacks *******
    glfwSetKeyCallback(window, keyCallback);
    glfwSetCursorPosCallback(window, mouseCallback);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

    //******* Initialize scene *******
    scene.initialize();

    //******* Game loop *******
    double lastFrame = glfwGetTime();
    while (glfwWindowShouldClose(window) == 0) {
        // FPS timing/display
        double currentFrame = glfwGetTime();
        double deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        double fps = determineFPS(deltaTime);

#ifdef DEBUG
        std::cout << "Delta time: " << deltaTime << std::endl;
#endif

        // Handle input
        scene.update(window, deltaTime);

        // Tick physics engine
        physicsEngine.updateScene(scene, deltaTime);

        // Draw scene
        renderingEngine.renderScene(scene, fps);

        // Flip buffers and draw
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();

    return EXIT_SUCCESS;
}
