// Local Headers
#include "globals.hpp"
#include "Core/camera.hpp"
#include "Core/textrenderer.hpp"
#include "Core/scene.hpp"
#include "Core/gameobject.hpp"
#include "Core/model.hpp"
#include "Core/shader.hpp"
#include "Physics/debugdrawer.hpp"
#include "Objects/car.hpp"
#include "Objects/terrain.hpp"
#include "Objects/wall.hpp"
#include "Objects/streetlight.hpp"

// System Headers
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// Standard Headers
#include <cstdio>
#include <cstdlib>
#include <iomanip>
#include <memory>
#include <iostream>
#include <sstream>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>



const int FPS_ROLLING_FRAMES = 10;

// Below used by other files
GLsizei width = 1280;
GLsizei height = 800;
GLfloat trackInnerA = 58;
GLfloat trackInnerB = 58;
GLfloat trackOuterA = 63;
GLfloat trackOuterB = 63;
std::shared_ptr<Core::Shader> defaultShader;
std::shared_ptr<Core::Shader> simpleShader;
std::unique_ptr<Core::TextRenderer> textRenderer;
glm::vec3 defaultTextColor;
std::unique_ptr<btDiscreteDynamicsWorld> dynamicsWorld;
std::unique_ptr<Physics::DebugDrawer> debugDrawer;

// Below local to translation unit
btDefaultCollisionConfiguration collisionConfiguration;
btCollisionDispatcher dispatcher(&collisionConfiguration);
btDbvtBroadphase overlappingPairCache;
btSequentialImpulseConstraintSolver solver;

Core::Scene scene;
bool firstMouse = true;
double lastX;
double lastY;


void errorCallback(int error, const char* description) {
    fprintf(stderr, "Error: %s\n", description);
}

void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    // Process-once key events should happen here
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }

    if (key == GLFW_KEY_P && action == GLFW_PRESS) {
        scene.getCamera()->cycleProjectionMode();
    }
    if (key == GLFW_KEY_R && action == GLFW_PRESS) {
        scene.getCamera()->restoreDefaults();
        scene.getCar()->restoreDefaults();
    }
    if (key == GLFW_KEY_F && action == GLFW_PRESS) {
        scene.cycleFog();
    }
    if (key == GLFW_KEY_T && action == GLFW_PRESS) {
        debugDrawer->cycleWillDraw();
    }
    if (key == GLFW_KEY_G && action == GLFW_PRESS) {
        scene.getCamera()->cycleRelativeAngle();
    }
}


void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
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

void framebufferSizeCallback(GLFWwindow* window, int newWidth, int newHeight) {
    width = newWidth;
    height = newHeight;
    glViewport(0, 0, width, height);
}

int main(int argc, char * argv[]) {
    // Load GLFW
    glfwInit();

    // Register error callback
    glfwSetErrorCallback(errorCallback);

    // Create a window
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
    auto window = glfwCreateWindow(width, height, "Driving Scene - Jeffrey Everett", nullptr, nullptr);

    // Check for Valid Context
    if (window == nullptr) {
        fprintf(stderr, "Failed to Create OpenGL Context");
        return EXIT_FAILURE;
    }

    // Create Context and Load OpenGL Functions
    glfwMakeContextCurrent(window);
    gladLoadGL();
    fprintf(stderr, "OpenGL %s\n", glGetString(GL_VERSION));

    // Initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Enable/disable features
    glEnable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);

    // OpenGL settings
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Initialize remaining globals
    textRenderer = std::make_unique<Core::TextRenderer>(PROJECT_SOURCE_DIR "/Fonts/arial.ttf");
    defaultTextColor = glm::vec3(0,0,0);

    defaultShader = std::make_shared<Core::Shader>(
        PROJECT_SOURCE_DIR "/Shaders/VertexShaders/default.vert",
        PROJECT_SOURCE_DIR "/Shaders/FragmentShaders/default.frag"
    );
    defaultShader->use();
    defaultShader->setVec3("dirLight.direction", glm::vec3(-1,-1,-1));
    defaultShader->setVec4("fogColor", glm::vec4(1,1,1,1));

    simpleShader = std::make_shared<Core::Shader>(
            PROJECT_SOURCE_DIR "/Shaders/VertexShaders/simple.vert",
            PROJECT_SOURCE_DIR "/Shaders/FragmentShaders/simple.frag"
    );

    dynamicsWorld = std::make_unique<btDiscreteDynamicsWorld>(&dispatcher, &overlappingPairCache, &solver, &collisionConfiguration);
    debugDrawer = std::make_unique<Physics::DebugDrawer>();
    debugDrawer->setDebugMode(2);
    dynamicsWorld->setDebugDrawer(&(*debugDrawer));

    // Create camera
    auto camera = std::make_shared<Core::Camera>();

    // Create cubemap
    std::vector<std::string> darkFaces {
        PROJECT_SOURCE_DIR "/Textures/CubeMaps/DarkStormy/DarkStormyLeft2048.png",
        PROJECT_SOURCE_DIR "/Textures/CubeMaps/DarkStormy/DarkStormyRight2048.png",
        PROJECT_SOURCE_DIR "/Textures/CubeMaps/DarkStormy/DarkStormyUp2048.png",
        PROJECT_SOURCE_DIR "/Textures/CubeMaps/DarkStormy/DarkStormyDown2048.png",
        PROJECT_SOURCE_DIR "/Textures/CubeMaps/DarkStormy/DarkStormyFront2048.png",
        PROJECT_SOURCE_DIR "/Textures/CubeMaps/DarkStormy/DarkStormyBack2048.png"
    };
    auto cubeMap = std::make_shared<Core::CubeMap>(darkFaces);

    // Create scene
    scene.setCamera(camera);
    scene.setCubeMap(cubeMap);
    scene.setFog(false);
    scene.setNightMode(true);

    // Set up objects
    Objects::Wall::setup();
    Objects::Car::setup();
    Objects::Terrain::setup();
    Objects::Streetlight::setup();

    // Add GameObjects to scene
    auto wall = std::make_shared<Objects::Wall>();
    scene.add(wall);
    auto car = std::make_shared<Objects::Car>();
    scene.setCar(car);
    scene.getCamera()->setFollow(car);
    scene.add(car);
    auto terrain = std::make_shared<Objects::Terrain>();
    scene.add(terrain);
    float streetlightOffset = 0.5f;
    int repetitions = 12;
    for (int i = 0; i < repetitions; i++) {
        float theta = glm::radians(360.0f)*i/repetitions;
        glm::vec3 posStreetlight1 = glm::vec3((trackInnerA-streetlightOffset)*glm::cos(theta), 0, (trackInnerB-streetlightOffset)*glm::sin(theta));
        glm::vec3 posStreetlight2 = glm::vec3((trackOuterA+streetlightOffset)*glm::cos(theta), 0, (trackOuterB+streetlightOffset)*glm::sin(theta));

        auto streetlight1 = std::make_shared<Objects::Streetlight>(posStreetlight1, -theta+glm::radians(180.0f), false);
        scene.addStreetlight(streetlight1);
        scene.add(streetlight1);

        auto streetlight2 = std::make_shared<Objects::Streetlight>(posStreetlight2, -theta+glm::radians(180.0f), true);
        scene.addStreetlight(streetlight2);
        scene.add(streetlight2);
    }

    // Register remaining callbacks
    glfwSetKeyCallback(window, keyCallback);
    glfwSetCursorPosCallback(window, mouseCallback);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

    double deltaTime;
    double lastFrame = glfwGetTime();
    double rollingFPS;
    double latestFrameDeltas[FPS_ROLLING_FRAMES];
    int latestFrameIdx = 0;
    bool firstPassFrames = true;

    // Rendering Loop
    while (glfwWindowShouldClose(window) == 0) {
        // Background Fill Color
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Start text renderer at bottom
        textRenderer->resetVerticalOffset();

        // FPS timing/display
        double currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
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
        scene.setDeltaTime(deltaTime);

#ifdef DEBUG
        std::cout << "Delta time: " << deltaTime << std::endl;
#endif

        // Tick physics engine
        dynamicsWorld->stepSimulation((float) deltaTime, 5);

        // Draw scene
        scene.draw();

        // Draw physics debugging
        dynamicsWorld->debugDrawWorld();

        // Render FPS as string
        std::ostringstream fpsOSS;
        fpsOSS << std::fixed << std::setprecision(5) << "FPS: " << rollingFPS;
        textRenderer->renderText(fpsOSS.str(), 1, defaultTextColor);

        // Process input
        scene.processInput(window);

        // Run object scripts
        scene.perFrame();

        // Flip Buffers and Draw
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    return EXIT_SUCCESS;
}
