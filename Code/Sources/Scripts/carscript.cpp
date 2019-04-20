#include "Scripts/carscript.hpp"

#include <iostream>

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
        mCarPhysicsBody = mGameObject.getComponents<Components::CarPhysicsBody>()[0];
    }

    void CarScript::onUpdate(GLFWwindow *window, float deltaTime)
    {
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
            mCarPhysicsBody->setBrake(BRAKE_FORCE);
        }
        else {
            mCarPhysicsBody->setBrake(0);
        }
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            std::cout << "w pressed" << std::endl;
            mCarPhysicsBody->applyEngineForce(ENGINE_FORCE);
        }
        else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            mCarPhysicsBody->applyEngineForce(-ENGINE_FORCE);
        }
        else {
            mCarPhysicsBody->applyEngineForce(0);
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            mCarPhysicsBody->setSteering((float)(mCarPhysicsBody->mSteering + WHEEL_TURN_RATE*deltaTime));
        }
        else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            mCarPhysicsBody->setSteering((float)(mCarPhysicsBody->mSteering - WHEEL_TURN_RATE*deltaTime));
        }
        else {
            // Eventually return to 0
            mCarPhysicsBody->setSteering(float((1-10*deltaTime)*mCarPhysicsBody->mSteering));
        }
    }
}
