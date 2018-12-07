#include "Objects/car.hpp"

using namespace Objects;


glm::vec3 Car::mSpotlightOffset1;
glm::vec3 Car::mSpotlightOffset2;
glm::vec3 Car::mTaillightOffset1;
glm::vec3 Car::mTaillightOffset2;
std::shared_ptr<Core::Drawable> Car::mDrawable;
std::shared_ptr<Core::Shader> Car::mShader;


Car::Car() : Core::GameObject(mDrawable, mShader), mWheelTurn(0) {
    setOffset(glm::vec3(0, -0.1, 0));
    scale(glm::vec3(SCALE_FACTOR, SCALE_FACTOR, SCALE_FACTOR));

    btBoxShape *carShape = new btBoxShape(btVector3(btScalar(0.25), btScalar(0.1), btScalar(0.55)));
    btTransform carTransform;
    carTransform.setIdentity();
    carTransform.setOrigin(btVector3(-trackInnerA - (trackOuterA-trackInnerA)/2, 1, 0));
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
    mDrawable = std::make_shared<Core::Model>(
        PROJECT_SOURCE_DIR "/Models/lambo/Lamborghini_Aventador.fbx"
    );
    mShader = defaultShader;

    defaultShader->use();
    // Front spotlights
    for (int i = 0; i <= 1; i++) {
        std::string number = std::to_string(i);
        defaultShader->setVec3("spotLights[" + number + "].ambient", glm::vec3(0.13,0.13,0.13));
        defaultShader->setVec3("spotLights[" + number + "].diffuse", glm::vec3(0.55,0.55,0.55));
        defaultShader->setVec3("spotLights[" + number + "].specular", glm::vec3(1,1,1));
        defaultShader->setFloat("spotLights[" + number + "].innerCutoff", glm::cos(glm::radians(12.5)));
        defaultShader->setFloat("spotLights[" + number + "].outerCutoff", glm::cos(glm::radians(17.5)));
        defaultShader->setFloat("spotLights[" + number + "].constant", 1.0f);
        defaultShader->setFloat("spotLights[" + number + "].linear", 0.09f);
        defaultShader->setFloat("spotLights[" + number + "].quadratic", 0.032f);
    }
    // Tail lights
    for (int i = 2; i <= 3; i++) {
        std::string number = std::to_string(i);
        defaultShader->setVec3("spotLights[" + number + "].ambient", glm::vec3(0.23,0.06,0.06));
        defaultShader->setVec3("spotLights[" + number + "].diffuse", glm::vec3(0.75,0.15,0.15));
        defaultShader->setVec3("spotLights[" + number + "].specular", glm::vec3(1,1,1));
        defaultShader->setFloat("spotLights[" + number + "].innerCutoff", glm::cos(glm::radians(12.5)));
        defaultShader->setFloat("spotLights[" + number + "].outerCutoff", glm::cos(glm::radians(17.5)));
        defaultShader->setFloat("spotLights[" + number + "].constant", 1.0f);
        defaultShader->setFloat("spotLights[" + number + "].linear", 0.09f);
        defaultShader->setFloat("spotLights[" + number + "].quadratic", 0.032f);
    }

    mSpotlightOffset1 = glm::vec3(1/SCALE_FACTOR)*glm::vec3(-0.17,0.15,0.5);
    mSpotlightOffset2 = glm::vec3(1/SCALE_FACTOR)*glm::vec3(0.17,0.15,0.5);
    mTaillightOffset1 = glm::vec3(1/SCALE_FACTOR)*glm::vec3(-0.17,0.15,-0.5);
    mTaillightOffset2 = glm::vec3(1/SCALE_FACTOR)*glm::vec3(0.17,0.15,-0.5);
}

void Car::calculateRotation(double angle) {
    mRigidBody->getWorldTransform().setRotation(btQuaternion(btVector3(0,1,0), glm::radians(angle)));
}

void Car::updateLighting() {
    // Spotlights
    glm::vec3 spotlightPosition1 = getPosition()+getWorldOffset(mSpotlightOffset1);
    glm::vec3 spotlightPosition2 = getPosition()+getWorldOffset(mSpotlightOffset2);
    glm::vec3 spotlightDirection = getWorldForward();

    defaultShader->use();
    defaultShader->setVec3("spotLights[0].position", spotlightPosition1);
    defaultShader->setVec3("spotLights[1].position", spotlightPosition2);
    defaultShader->setVec3("spotLights[0].direction", spotlightDirection);
    defaultShader->setVec3("spotLights[1].direction", spotlightDirection);

    debugDrawer->drawLine(glmVec32btVector3(spotlightPosition1), glmVec32btVector3(spotlightPosition1+glm::vec3(3)*spotlightDirection), btVector3(1.0,0,0));
    debugDrawer->drawLine(glmVec32btVector3(spotlightPosition2), glmVec32btVector3(spotlightPosition2+glm::vec3(3)*spotlightDirection), btVector3(1.0,0,0));

    // Tail lights
    glm::vec3 taillightPosition1 = getPosition()+getWorldOffset(mTaillightOffset1);
    glm::vec3 taillightPosition2 = getPosition()+getWorldOffset(mTaillightOffset2);
    glm::vec3 taillightDirection = glm::vec3(-1)*getWorldForward();

    defaultShader->use();
    defaultShader->setVec3("spotLights[2].position", taillightPosition1);
    defaultShader->setVec3("spotLights[3].position", taillightPosition2);
    defaultShader->setVec3("spotLights[2].direction", taillightDirection);
    defaultShader->setVec3("spotLights[3].direction", taillightDirection);

    debugDrawer->drawLine(glmVec32btVector3(taillightPosition1), glmVec32btVector3(taillightPosition1+glm::vec3(3)*taillightDirection), btVector3(1.0,0,0));
    debugDrawer->drawLine(glmVec32btVector3(taillightPosition2), glmVec32btVector3(taillightPosition2+glm::vec3(3)*taillightDirection), btVector3(1.0,0,0));
}

void Car::processInput(GLFWwindow *window, double deltaTime) {
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        setPosition(glm::vec3(-trackInnerA - (trackOuterA-trackInnerA)/2, 1, 0));
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
