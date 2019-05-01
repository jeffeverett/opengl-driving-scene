#include "Scripts/carscript.hpp"
#include "Utils/logger.hpp"

#include <iostream>

const glm::quat INITIAL_ROTATION = glm::angleAxis(glm::radians(180.0f), glm::vec3(0, 1, 0));
const float INITIAL_STEERING = 0.0f;

const float ENGINE_FORCE = 600.0f;
const float BRAKE_FORCE = 100.0f;
const float WHEEL_TURN_RATE = 0.2f;

namespace Scripts
{
    CarScript::CarScript(Core::GameObject &gameObject, glm::vec3 initialPosition) : Script(gameObject),
        mInitialPosition(initialPosition), mTimeSinceLastSpawnedPS(1e6), mNextPoolIdx(0)
    {
        auto particleUpdateShader = std::make_shared<Assets::Shader>(
            PROJECT_SOURCE_DIR "/Shaders/ComputeShaders/flames_update.cs"
        );
        auto particleRenderShader = std::make_shared<Assets::Shader>(
            PROJECT_SOURCE_DIR "/Shaders/VertexShaders/flames_render.vert",
            PROJECT_SOURCE_DIR "/Shaders/GeometryShaders/flames_render.geom",
            PROJECT_SOURCE_DIR "/Shaders/FragmentShaders/flames_render.frag"
        );
        auto particleSystem = std::make_shared<Assets::ParticleSystem>();
        particleSystem->mParticleLifetime = 1.0f;
        particleSystem->mInitialParticleSize = glm::vec2(0.5f, 0.5f);
        particleSystem->mFinalParticleSize = glm::vec2(0.2f, 0.2f);
        particleSystem->mUpdateShader = particleUpdateShader;
        particleSystem->mRenderShader = particleRenderShader;
        particleSystem->mTextures.push_back(std::make_shared<Assets::Texture>(PROJECT_SOURCE_DIR "/Textures/Particles/flames.tga"));
        particleSystem->mColors.push_back(glm::vec3(0.886, 0.345, 0.133));
        particleSystem->mColors.push_back(glm::vec3(0.0, 0.0, 1.0));
        mParticleSystem = particleSystem;
    }

    CarScript::~CarScript()
    {
    }

    void CarScript::onStart(Core::Scene &scene)
    {
        mCarPhysicsBody = mGameObject.getComponents<Components::CarPhysicsBody>()[0];
        mGameObject.mTransform->setRotation(INITIAL_ROTATION);

        for (int i = 0; i < PSR_POOL_SIZE; i++) {
            // Create gameobject
            auto psrGameObject = std::make_shared<Core::GameObject>(mGameObject.mTransform->getWorldTranslation());

            // Create paricle system renderer
            auto particleSystemRenderer = std::make_shared<Components::ParticleSystemRenderer>(*psrGameObject);
            particleSystemRenderer->mIsActive = false;
            particleSystemRenderer->mTimeActive = 0.0f;
            particleSystemRenderer->mParticleSystem = mParticleSystem;
            particleSystemRenderer->setupParticleSystem(100);
            psrGameObject->addComponent(particleSystemRenderer);

            // Add to scene
            scene.add(psrGameObject);

            // Add to pool
            mParticleSystemRendererPool[i] = particleSystemRenderer;
        }
    }

    void CarScript::onUpdate(GLFWwindow *window, Core::Scene &scene, float deltaTime)
    {
        mTimeSinceLastSpawnedPS += deltaTime;

        // Key for resetting car position and position
        if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
            mGameObject.mTransform->setTranslation(mInitialPosition);
            mGameObject.mTransform->setRotation(INITIAL_ROTATION);
            mCarPhysicsBody->setSteering(INITIAL_STEERING);
            mCarPhysicsBody->mRigidBody->setLinearVelocity(btVector3(0,0,0));
            mCarPhysicsBody->mRigidBody->setAngularVelocity(btVector3(0,0,0));
        }

        #ifdef DEBUG
            Utils::Logger::log("Car position", mGameObject.mTransform->mTranslation);
            Utils::Logger::log("Car world position", mGameObject.mTransform->getWorldTranslation());
        #endif

        // Keys for driving car
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
            mCarPhysicsBody->setBrake(BRAKE_FORCE);
        }
        else {
            mCarPhysicsBody->setBrake(0);
        }
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            mCarPhysicsBody->applyEngineForce(ENGINE_FORCE);
            
            if (mTimeSinceLastSpawnedPS > 0.01f) {
                mParticleSystemRendererPool[mNextPoolIdx]->mGameObject.mTransform->setTranslation(mGameObject.mTransform->getWorldTranslation());
                mParticleSystemRendererPool[mNextPoolIdx]->mIsActive = true;
                mParticleSystemRendererPool[mNextPoolIdx]->mTimeActive = 0.0f;

                mNextPoolIdx = (mNextPoolIdx + 1) % PSR_POOL_SIZE;
                mTimeSinceLastSpawnedPS = 0.0f;
            }
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
