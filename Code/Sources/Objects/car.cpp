#include "Objects/car.hpp"

using namespace Objects;


glm::vec3 Car::mSpotlightOffset1;
glm::vec3 Car::mSpotlightOffset2;
std::shared_ptr<Utils::Drawable> Car::mDrawable;
std::shared_ptr<Utils::Shader> Car::mShader;


Car::Car() : Utils::GameObject(mDrawable, mShader), mWheelTurn(0) {
    setOffset(glm::vec3(0, -0.1, 0));
    scale(glm::vec3(SCALE_FACTOR, SCALE_FACTOR, SCALE_FACTOR));

    btBoxShape *carShape = new btBoxShape(btVector3(btScalar(0.25), btScalar(0.1), btScalar(0.55)));
    btTransform carTransform;
    carTransform.setIdentity();
    carTransform.setOrigin(btVector3(0, 2, 0));
    btScalar mass(1575.0);
    btVector3 localInertia(0, 0, 0);
    carShape->calculateLocalInertia(mass, localInertia);
    btDefaultMotionState *myMotionState = new btDefaultMotionState(carTransform);
    btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, carShape, localInertia);
    mRigidBody = std::make_unique<btRigidBody>(rbInfo);
    dynamicsWorld->addRigidBody(&(*mRigidBody));

    calculateRotation(180);
}

Car::~Car() { }


// Public Member Functions
void Car::setup() {
    mDrawable = std::make_shared<Utils::Model>(
        PROJECT_SOURCE_DIR "/Models/lambo/Lamborghini_Aventador.fbx"
    );
    mShader = defaultShader;

    defaultShader->use();
    defaultShader->setVec3("spotLights[0].ambient", glm::vec3(0.1,0.1,0.1));
    defaultShader->setVec3("spotLights[0].diffuse", glm::vec3(0.35,0.35,0.35));
    defaultShader->setVec3("spotLights[0].specular", glm::vec3(1,1,1));
    defaultShader->setFloat("spotLights[0].innerCutoff", glm::cos(glm::radians(12.5)));
    defaultShader->setFloat("spotLights[0].outerCutoff", glm::cos(glm::radians(17.5)));
    defaultShader->setFloat("spotLights[0].constant", 1.0f);
    defaultShader->setFloat("spotLights[0].linear", 0.09f);
    defaultShader->setFloat("spotLights[0].quadratic", 0.032f);
    /*defaultShader->setVec3("spotLights[1].ambient", glm::vec3(0.1,0.1,0.1));
    defaultShader->setVec3("spotLights[1].diffuse", glm::vec3(0.35,0.35,0.35));
    defaultShader->setVec3("spotLights[1].specular", glm::vec3(1,1,1));
    defaultShader->setFloat("spotLights[1].innerCutoff", glm::radians(5.0));
    defaultShader->setFloat("spotLights[1].outerCutoff", glm::radians(7.0));*/
}

void Car::calculateRotation(double angle) {
    mRigidBody->getWorldTransform().setRotation(btQuaternion(btVector3(0,1,0), glm::radians(angle)));
}

void Car::updateLighting() {
    glm::vec3 spotlightPosition1 = getPosition()+mSpotlightOffset1;
    glm::vec3 spotlightPosition2 = getPosition()+mSpotlightOffset2;
    glm::vec3 spotlightDirection = getWorldForward();

    defaultShader->use();
    defaultShader->setVec3("spotLights[0].position", spotlightPosition1);
    defaultShader->setVec3("spotLights[1].position", spotlightPosition2);
    defaultShader->setVec3("spotLights[0].direction", spotlightDirection);
    defaultShader->setVec3("spotLights[1].direction", spotlightDirection);

    debugDrawer->drawLine(glmVec32btVector3(spotlightPosition1), glmVec32btVector3(spotlightPosition1+glm::vec3(3)*spotlightDirection), btVector3(1.0,0,0));
    //debugDrawer->drawLine(glmVec32btVector3(spotlightPosition2), glmVec32btVector3(spotlightPosition2+glm::vec3(3)*spotlightDirection), btVector3(1.0,0,0));
}

void Car::processInput(GLFWwindow *window, double deltaTime) {
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        setPosition(glm::vec3(0,2,0));
        calculateRotation(180);
        mRigidBody->setLinearVelocity(btVector3(0,0,0));
        mRigidBody->setAngularVelocity(btVector3(0,0,0));
    }
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        applyForce(glm::vec3(glm::sin(glm::radians(mTheta))*ACCELERATION, 0, glm::cos(glm::radians(mTheta))*ACCELERATION));
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        applyForce(glm::vec3(-glm::sin(glm::radians(mTheta))*ACCELERATION, 0, -glm::cos(glm::radians(mTheta))*ACCELERATION));

    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        mWheelTurn -= WHEEL_TURN_RATE * deltaTime;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        mWheelTurn += WHEEL_TURN_RATE * deltaTime;
    }

    // https://pybullet.org/Bullet/phpBB3/viewtopic.php?t=12045
    mRigidBody->setActivationState(ACTIVE_TAG);
}

void Car::perFrame(double deltaTime) {
    std::ostringstream cpOSS;
    glm::vec3 position = getPosition();
    cpOSS << std::fixed << std::setprecision(5) << "Car Pos: (" << position[0] << ", " << position[1] << ", " << position[2] << ")";
    textRenderer->renderText(cpOSS.str(), 1, glm::vec3(0.5,0.5,0.5));

    std::ostringstream ctOSS;
    ctOSS << std::fixed << std::setprecision(5) << "Car Theta: " << fmod(mTheta, 360);
    textRenderer->renderText(ctOSS.str(), 1, glm::vec3(0.5,0.5,0.5));
}
