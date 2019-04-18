#include "Scripts/carscript.hpp"

#include <iostream>
#include <Components/carphysicsbody.hpp>

const float ENGINE_FORCE = 600.0f;
const float BRAKE_FORCE = 100.0f;
const float WHEEL_TURN_RATE = 0.2f;

namespace Scripts
{
    CarScript::CarScript(const Core::GameObject &gameObject) : Script(gameObject)
    {
    }

    CarScript::~CarScript()
    {
    }

    void CarScript::onStart()
    {
      // vroom vroom
    }

    void CarScript::onUpdate(GLFWwindow *window, float deltaTime)
    {
        auto carPhysicsBody = mGameObject.getComponents<Components::CarPhysicsBody>()[0];

        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
            carPhysicsBody->setBrake(BRAKE_FORCE);
        }
        else {
            carPhysicsBody->setBrake(0);
        }
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            std::cout << "w pressed" << std::endl;
            carPhysicsBody->applyEngineForce(ENGINE_FORCE);
        }
        else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            carPhysicsBody->applyEngineForce(-ENGINE_FORCE);
        }
        else {
            carPhysicsBody->applyEngineForce(0);
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            carPhysicsBody->setSteering((float)(carPhysicsBody->mSteering + WHEEL_TURN_RATE*deltaTime));
        }
        else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            carPhysicsBody->setSteering((float)(carPhysicsBody->mSteering - WHEEL_TURN_RATE*deltaTime));
        }
        else {
            // Eventually return to 0
            carPhysicsBody->setSteering(float((1-10*deltaTime)*carPhysicsBody->mSteering));
        }
    }
}
