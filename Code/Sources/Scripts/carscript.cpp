#include "Scripts/carscript.hpp"

#include <iostream>

const glm::vec3 INITIAL_POS(0, 0, 0);
const glm::vec3 INITIAL_ROTATION(0, 180.0f, 0);
const float INITIAL_STEERING = 0.0f;

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

        mGameObject.mTransform->setTranslation(INITIAL_POS);
        mGameObject.mTransform->setRotation(INITIAL_ROTATION);
    }

    void CarScript::onUpdate(GLFWwindow *window, float deltaTime)
    {
        // Key for resetting car position and position
        if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
            mGameObject.mTransform->setTranslation(INITIAL_POS);
            mGameObject.mTransform->setRotation(INITIAL_ROTATION);
            mCarPhysicsBody->setSteering(INITIAL_STEERING);
            mCarPhysicsBody->mRigidBody->setLinearVelocity(btVector3(0,0,0));
            mCarPhysicsBody->mRigidBody->setAngularVelocity(btVector3(0,0,0));
        }

        // Keys for driving car
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
