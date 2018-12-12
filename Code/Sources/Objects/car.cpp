#include "Objects/car.hpp"

using namespace Objects;


glm::vec3 Car::mSpotlightOffset1;
glm::vec3 Car::mSpotlightOffset2;
glm::vec3 Car::mTaillightOffset1;
glm::vec3 Car::mTaillightOffset2;
std::shared_ptr<Core::Model> Car::mModel;
std::shared_ptr<Core::Drawable> Car::mDrawable;
std::shared_ptr<Core::Shader> Car::mShader;


const glm::vec3 START_POS(-trackInnerA - (trackOuterA-trackInnerA)/2, 1, 0);
const float START_ROTATION = 180.0f;
const float START_STEERING = 0.0f;

const float SCALE_FACTOR = 1.0f/400.0f;
const float ENGINE_FORCE = 600.0f;
const float BRAKE_FORCE = 100.0f;
const float WHEEL_TURN_RATE = 0.2f;

const float MASS = 800.0f;

const float CHASSIS_WIDTH = 0.25f;
const float CHASSIS_LENGTH = 0.55f;
const float CHASSIS_HEIGHT = 0.09f;
const float WHEEL_OFFSET_FRONT = 0.23f;
const float WHEEL_OFFSET_BACK = 0.18f;
const float WHEEL_OFFSET_SIDE = 0.02f;

const float CONNECTION_HEIGHT = 0.16f;

const float WHEEL_WIDTH = 0.03f;
const float WHEEL_RADIUS = 0.095f;

const float STEERING_CLAMP = 0.3f;

const float	WHEEL_FRICTION = 1000.0f;
const float SUSPENSION_STIFFNESS = 50.f;
const float SUSPENSION_DAMPING = 4.3f;
const float SUSPENSION_COMPRESSION = 4.4f;
const float SUSPENSION_REST_LENGTH = 0.12f;
const float	ROLL_INFLUENCE = 0.02f;

Car::Car() : Core::GameObject(mDrawable, mShader) {
    setOffset(glm::vec3(0, -0.1, 0));
    scale(glm::vec3(SCALE_FACTOR, SCALE_FACTOR, SCALE_FACTOR));

    mChassisShape = std::make_unique<btBoxShape>(btVector3(CHASSIS_WIDTH, CHASSIS_HEIGHT, CHASSIS_LENGTH));
    mShape = std::make_unique<btCompoundShape>();
    btTransform localTrans;
    localTrans.setIdentity();
    //localTrans effectively shifts the center of mass with respect to the chassis
    localTrans.setOrigin(btVector3(0,0.15,0));
    static_cast<btCompoundShape*>(&(*mShape))->addChildShape(localTrans, &(*mChassisShape));

    btTransform carTransform;
    carTransform.setIdentity();
    carTransform.setOrigin(glmVec32btVector3(START_POS));
    btScalar mass(MASS);
    btVector3 localInertia(0, 0, 0);
    mShape->calculateLocalInertia(mass, localInertia);
    mMotionState = std::make_unique<btDefaultMotionState>(carTransform);
    btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, &(*mMotionState), &(*mShape), localInertia);
    mRigidBody = std::make_unique<btRigidBody>(rbInfo);
    dynamicsWorld->addRigidBody(&(*mRigidBody));

    mVehicleRaycaster = std::make_shared<btDefaultVehicleRaycaster>(&(*dynamicsWorld));
    btRaycastVehicle::btVehicleTuning tuning;
    mVehicle = std::make_shared<btRaycastVehicle>(tuning, &(*mRigidBody), &(*mVehicleRaycaster));
    mRigidBody->setActivationState(DISABLE_DEACTIVATION);
    dynamicsWorld->addVehicle(&(*mVehicle));

    btVector3 wheelDirection(0,-1,0);
    btVector3 wheelAxleCS(-1,0,0);
    std::vector<btVector3> connectionPoints;
    connectionPoints.push_back(btVector3(-CHASSIS_WIDTH-WHEEL_OFFSET_SIDE, CONNECTION_HEIGHT, CHASSIS_LENGTH-WHEEL_OFFSET_FRONT));
    connectionPoints.push_back(btVector3(CHASSIS_WIDTH+WHEEL_OFFSET_SIDE, CONNECTION_HEIGHT, CHASSIS_LENGTH-WHEEL_OFFSET_FRONT));
    connectionPoints.push_back(btVector3(-CHASSIS_WIDTH-WHEEL_OFFSET_SIDE, CONNECTION_HEIGHT, -CHASSIS_LENGTH+WHEEL_OFFSET_BACK));
    connectionPoints.push_back(btVector3(CHASSIS_WIDTH+WHEEL_OFFSET_SIDE, CONNECTION_HEIGHT, -CHASSIS_LENGTH+WHEEL_OFFSET_BACK));


    for (auto &connectionPoint : connectionPoints) {
        mVehicle->addWheel(
            connectionPoint,
            wheelDirection,
            wheelAxleCS,
            SUSPENSION_REST_LENGTH,
            WHEEL_RADIUS,
            tuning,
            connectionPoint[2] > 0
        );
    }

    for (int i=0; i < mVehicle->getNumWheels(); i++) {
        btWheelInfo& wheel = mVehicle->getWheelInfo(i);
        wheel.m_suspensionStiffness = SUSPENSION_STIFFNESS;
        wheel.m_wheelsDampingRelaxation = SUSPENSION_DAMPING;
        wheel.m_wheelsDampingCompression = SUSPENSION_COMPRESSION;
        wheel.m_frictionSlip = WHEEL_FRICTION;
        wheel.m_rollInfluence = ROLL_INFLUENCE;
    }

    setRotation(START_ROTATION);
    setSteering(START_STEERING);
    mVehicle->setCoordinateSystem(0, 1, 2);
}

Car::~Car() {
    dynamicsWorld->removeVehicle(&(*mVehicle));
}

void Car::setup() {
    mModel = std::make_shared<Core::Model>(PROJECT_SOURCE_DIR "/Models/lambo/Lamborghini_Aventador.fbx");
    mDrawable = mModel;
    mShader = defaultShader;

    // Center wheels
    for (int i = 2; i <= 5; i++) {
        mModel->meshes[i].center();
    }

    defaultShader->use();
    // Front spotlights
    for (int i = 0; i <= 1; i++) {
        std::string number = std::to_string(i);
        defaultShader->setVec3("spotLights[" + number + "].ambient", glm::vec3(0.2,0.2,0.2));
        defaultShader->setVec3("spotLights[" + number + "].diffuse", glm::vec3(0.75,0.75,0.75));
        defaultShader->setVec3("spotLights[" + number + "].specular", glm::vec3(1,1,1));
        defaultShader->setFloat("spotLights[" + number + "].innerCutoff", glm::cos(glm::radians(12.5f)));
        defaultShader->setFloat("spotLights[" + number + "].outerCutoff", glm::cos(glm::radians(17.5f)));
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
        defaultShader->setFloat("spotLights[" + number + "].innerCutoff", glm::cos(glm::radians(12.5f)));
        defaultShader->setFloat("spotLights[" + number + "].outerCutoff", glm::cos(glm::radians(17.5f)));
        defaultShader->setFloat("spotLights[" + number + "].constant", 1.0f);
        defaultShader->setFloat("spotLights[" + number + "].linear", 0.09f);
        defaultShader->setFloat("spotLights[" + number + "].quadratic", 0.032f);
    }

    mSpotlightOffset1 = glm::vec3(1/SCALE_FACTOR)*glm::vec3(-0.17,0.15,0.5);
    mSpotlightOffset2 = glm::vec3(1/SCALE_FACTOR)*glm::vec3(0.17,0.15,0.5);
    mTaillightOffset1 = glm::vec3(1/SCALE_FACTOR)*glm::vec3(-0.17,0.15,-0.5);
    mTaillightOffset2 = glm::vec3(1/SCALE_FACTOR)*glm::vec3(0.17,0.15,-0.5);

}

void Car::restoreDefaults() {
    setPosition(START_POS);
    setRotation(START_ROTATION);
    setSteering(START_STEERING);
    mRigidBody->setLinearVelocity(btVector3(0,0,0));
    mRigidBody->setAngularVelocity(btVector3(0,0,0));
}

void Car::draw() {
    mShader->use();

    btScalar modelTransform[16];
    mRigidBody->getWorldTransform().getOpenGLMatrix(modelTransform);
    glm::mat4 modelMtx = btScalar2glmMat4(modelTransform);
    glm::vec3 worldOffset = getWorldOffset();
    modelTransform[12] += worldOffset[0];
    modelTransform[13] += worldOffset[1];
    modelTransform[14] += worldOffset[2];
    modelMtx = glm::scale(modelMtx, mScale);
    mShader->setMat4("model", modelMtx);


    // Draw main chassis
    mModel->meshes[0].draw(*mShader); // main chassis
    mModel->meshes[1].draw(*mShader); // glass paneling


    // Draw wheels (same order as connection points)
    for (int i = 2; i <= 5; i++) {
        btScalar wheelTransform[16];
        mVehicle->getWheelInfo(i-2).m_worldTransform.getOpenGLMatrix(wheelTransform);
        glm::mat4 wheelMtx = btScalar2glmMat4(wheelTransform);
        wheelMtx = glm::scale(wheelMtx, mScale);
        mShader->setMat4("model", wheelMtx);

        mModel->meshes[i].draw(*mShader);
    }
}


void Car::createCircularFace(Core::MeshCreator &meshCreator, float xOffset) {
    // Create circular faces
    int thetaPartitions = 20;
    float theta0 = 0;
    for (int i = 1; i<= thetaPartitions; i++) {
        float theta = glm::radians(360.0f*i/thetaPartitions);

        // Positions
        glm::vec3 center(xOffset, 0, 0);
        glm::vec3 left(xOffset, WHEEL_RADIUS*glm::sin(theta0), WHEEL_RADIUS*glm::cos(theta0));
        glm::vec3 right(xOffset, WHEEL_RADIUS*glm::sin(theta), WHEEL_RADIUS*glm::cos(theta));

        // Create triangle
        meshCreator.mPositions.push_back(left);
        meshCreator.mPositions.push_back(center);
        meshCreator.mPositions.push_back(right);
        meshCreator.mNormals.push_back(glm::vec3(0, left[1], left[2]));
        meshCreator.mNormals.push_back(glm::vec3(0, center[1], center[2]));
        meshCreator.mNormals.push_back(glm::vec3(0, right[1], right[2]));
        meshCreator.mTexCoords.push_back(glm::vec2(left[1]/WHEEL_RADIUS, left[2]/WHEEL_RADIUS));
        meshCreator.mTexCoords.push_back(glm::vec2(center[1]/WHEEL_RADIUS, center[2]/WHEEL_RADIUS));
        meshCreator.mTexCoords.push_back(glm::vec2(right[1]/WHEEL_RADIUS, right[2]/WHEEL_RADIUS));

        theta0 = theta;
    }
}
void Car::createWheel(Core::MeshCreator &meshCreator) {
    // Create circular faces
    createCircularFace(meshCreator, WHEEL_WIDTH);
    createCircularFace(meshCreator, -WHEEL_WIDTH);
}

void Car::applyEngineForce(float force) {
    for (int i=0; i < mVehicle->getNumWheels(); i++) {
        btWheelInfo &wheel = mVehicle->getWheelInfo(i);
        // 4WD
        mVehicle->applyEngineForce(force, i);
    }
}

void Car::setBrake(float force) {
    for (int i=0; i < mVehicle->getNumWheels(); i++) {
        btWheelInfo &wheel = mVehicle->getWheelInfo(i);
        // 4WD
        mVehicle->setBrake(force, i);
    }
}

void Car::setSteering(float steering) {
    mSteering = steering;
    if (mSteering > STEERING_CLAMP) {
        mSteering = STEERING_CLAMP;
    }
    else if (mSteering < -STEERING_CLAMP) {
        mSteering = -STEERING_CLAMP;
    }
    for (int i=0; i < mVehicle->getNumWheels(); i++) {
        btWheelInfo &wheel = mVehicle->getWheelInfo(i);
        // Steer with front wheels
        if (wheel.m_bIsFrontWheel) {
            mVehicle->setSteeringValue(mSteering, i);
        }
    }
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

    debugDrawer->drawLine(glmVec32btVector3(taillightPosition1), glmVec32btVector3(taillightPosition1+glm::vec3(1)*taillightDirection), btVector3(1.0,0,0));
    debugDrawer->drawLine(glmVec32btVector3(taillightPosition2), glmVec32btVector3(taillightPosition2+glm::vec3(1)*taillightDirection), btVector3(1.0,0,0));
}

void Car::processInput(GLFWwindow *window, double deltaTime) {
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        setBrake(BRAKE_FORCE);
    }
    else {
        setBrake(0);
    }
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        applyEngineForce(ENGINE_FORCE);
    }
    else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        applyEngineForce(-ENGINE_FORCE);
    }
    else {
        applyEngineForce(0);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        setSteering((float)(mSteering + WHEEL_TURN_RATE*deltaTime));
    }
    else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        setSteering((float)(mSteering - WHEEL_TURN_RATE*deltaTime));
    }
    else {
        // Eventually return to 0
        setSteering(float((1-10*deltaTime)*mSteering));
    }

    // https://pybullet.org/Bullet/phpBB3/viewtopic.php?t=12045
    mRigidBody->setActivationState(ACTIVE_TAG);
}

void Car::perFrame(double deltaTime) {
    std::ostringstream cpOSS;
    glm::vec3 position = getPosition();
    cpOSS << std::fixed << std::setprecision(5) << "Car Pos: (" << position[0] << ", " << position[1] << ", " << position[2] << ")";
    textRenderer->renderText(cpOSS.str(), 1, defaultTextColor);
}
