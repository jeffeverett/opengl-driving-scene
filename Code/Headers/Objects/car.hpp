#pragma once

// Local Headers
#include "globals.hpp"
#include "Utils/gameobject.hpp"
#include "Utils/model.hpp"

// System Headers
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

// Standard Headers
#include <vector>
#include <memory>
#include <iomanip>

const double SCALE_FACTOR = 1.0/400.0;
const double MAX_SPEED = 10;
const double MAX_ANGULAR_SPEED = 10;
const double ACCELERATION = 12;
const double ANGULAR_ACCELERATION = 15;
const double FRICTION = 6;

// Define Namespace
namespace Objects
{
    class Car : public Utils::GameObject
    {
    public:

        // Implement Default Constructor and Destructor
        Car() : Utils::GameObject(mDrawable, mShader) {
            // Call setup before constructor

            scale(glm::vec3(SCALE_FACTOR, SCALE_FACTOR, SCALE_FACTOR));
            rotate(180.0f);
        }
        ~Car() { }


        // Public Member Functions
        static void setup() {
            mDrawable = std::make_shared<Utils::Model>(
                PROJECT_SOURCE_DIR "/Models/lambo/Lamborghini_Aventador.fbx"
            );
            mShader = defaultShader;
        }


        void processInput(GLFWwindow *window, double deltaTime) override {
            if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
                mAcceleration = ACCELERATION;
            }
            if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
                mAcceleration = -ACCELERATION;
            }
            if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
                mAngularAcceleration = ANGULAR_ACCELERATION;
            }
            if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
                mAngularAcceleration = -ANGULAR_ACCELERATION;
            }
        }

        void perFrame(double deltaTime) override {
            mAngularSpeed += mAngularAcceleration*deltaTime;
            if (mAngularSpeed > MAX_ANGULAR_SPEED) {
                mAngularSpeed = MAX_ANGULAR_SPEED;
            }
            else if (mAngularSpeed < -MAX_ANGULAR_SPEED) {
                mAngularSpeed = -MAX_ANGULAR_SPEED;
            }
            if (glm::abs(mAngularSpeed) > 0.05) {
                rotate(mAngularSpeed*deltaTime);

                if (mAngularSpeed > 0) {
                    mAngularSpeed -= FRICTION*deltaTime;
                }
                else {
                    mAngularSpeed += FRICTION*deltaTime;
                }
            }

            mSpeed += mAcceleration*deltaTime;
            if (mSpeed > MAX_SPEED) {
                mSpeed = MAX_SPEED;
            }
            else if (mSpeed < -MAX_SPEED) {
                mSpeed = -MAX_SPEED;
            }

            if (glm::abs(mSpeed) > 0.05) {
                glm::vec3 positionOffset = glm::vec3(mSpeed * glm::sin(glm::radians(mTheta)) * deltaTime, 0, mSpeed * glm::cos(glm::radians(mTheta)) * deltaTime);
                translate(positionOffset);

                if (mSpeed > 0) {
                    mSpeed -= FRICTION*deltaTime;
                }
                else {
                    mSpeed += FRICTION*deltaTime;
                }
            }

            mAcceleration = 0;
            mAngularAcceleration = 0;

            std::ostringstream cpOSS;
            glm::vec3 position = getPosition();
            cpOSS << std::fixed << std::setprecision(5) << "Car Pos: (" << position[0] << ", " << position[1] << ", " << position[2] << ")";
            textRenderer->renderText(cpOSS.str(), 1, glm::vec3(0.5,0.5,0.5));

            std::ostringstream csOSS;
            csOSS << std::fixed << std::setprecision(4) << "Speed: " << mSpeed << ", Ang Speed: " << mAngularSpeed << ", Theta: " << mTheta << ", Acc: " << mAcceleration << ", Ang Acc: " << mAngularAcceleration;
            textRenderer->renderText(csOSS.str(), 1, glm::vec3(0.5,0.5,0.5));
        }

    private:

        // Disable Copying and Assignment
        Car(Car const &) = delete;
        Car & operator=(Car const &) = delete;

        // Private members
        double mAngularSpeed = 0;
        double mAngularAcceleration = 0;

        double mSpeed = 0;
        double mAcceleration = 0;

        static std::shared_ptr<Utils::Drawable> mDrawable;
        static std::shared_ptr<Utils::Shader> mShader;
    };

    std::shared_ptr<Utils::Drawable> Car::mDrawable;
    std::shared_ptr<Utils::Shader> Car::mShader;
}
