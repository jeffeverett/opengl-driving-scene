#include "Objects/car.hpp"

using namespace Objects;


glm::vec3 Car::mSpotlightOffset1;
glm::vec3 Car::mSpotlightOffset2;
glm::vec3 Car::mTaillightOffset1;
glm::vec3 Car::mTaillightOffset2;
std::shared_ptr<Core::Mesh> Car::mWheelMesh;
std::shared_ptr<Core::Drawable> Car::mDrawable;
std::shared_ptr<Core::Shader> Car::mShader;


const glm::vec3 START_POS(-trackInnerA - (trackOuterA-trackInnerA)/2, 3, 0);
const float START_ROTATION = 180.0f;

const double SCALE_FACTOR = 1.0/400.0;
const double ENGINE_FORCE = 300.0;
const double WHEEL_TURN_RATE = 1.5;

const float MASS = 800.0f;

const float CHASSIS_WIDTH = 0.25f;
const float CHASSIS_LENGTH = 0.55f;
const float CHASSIS_HEIGHT = 0.1f;

const float CONNECTION_HEIGHT = 0.03f;

const float WHEEL_WIDTH = 0.03f;
const float WHEEL_RADIUS = 0.08f;

const float STEERING_CLAMP = 0.3f;

const float	WHEEL_FRICTION = 1000.0f;
const float SUSPENSION_STIFFNESS = 10.f;
const float SUSPENSION_DAMPING = 2.3f;
const float SUSPENSION_COMPRESSION = 4.4f;
const float SUSPENSION_REST_LENGTH = 0.4f;
const float	ROLL_INFLUENCE = 0.1f;

Car::Car() : Core::GameObject(mDrawable, mShader) {
    setOffset(glm::vec3(0, -0.1, 0));
    scale(glm::vec3(SCALE_FACTOR, SCALE_FACTOR, SCALE_FACTOR));

    btBoxShape *chassisShape = new btBoxShape(btVector3(CHASSIS_WIDTH, CHASSIS_HEIGHT, CHASSIS_LENGTH));
    btCompoundShape *compoundShape = new btCompoundShape();
    btTransform localTrans;
    localTrans.setIdentity();
    //localTrans effectively shifts the center of mass with respect to the chassis
    localTrans.setOrigin(btVector3(0,0.02,0));
    compoundShape->addChildShape(localTrans,chassisShape);

    btTransform carTransform;
    carTransform.setIdentity();
    carTransform.setOrigin(glmVec32btVector3(START_POS));
    btScalar mass(MASS);
    btVector3 localInertia(0, 0, 0);
    compoundShape->calculateLocalInertia(mass, localInertia);
    btDefaultMotionState *myMotionState = new btDefaultMotionState(carTransform);
    btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, compoundShape, localInertia);
    mRigidBody = std::make_unique<btRigidBody>(rbInfo);
    dynamicsWorld->addRigidBody(&(*mRigidBody));

    btCylinderShapeX *wheelShape = new btCylinderShapeX(btVector3(WHEEL_WIDTH, WHEEL_RADIUS, WHEEL_RADIUS));


    btVehicleRaycaster *vehicleRayCaster = new btDefaultVehicleRaycaster(&(*dynamicsWorld));
    btRaycastVehicle::btVehicleTuning tuning;
    mVehicle = new btRaycastVehicle(tuning, &(*mRigidBody), vehicleRayCaster);
    mRigidBody->setActivationState(DISABLE_DEACTIVATION);
    dynamicsWorld->addVehicle(mVehicle);

    btVector3 wheelDirection(0,-1,0);
    btVector3 wheelAxleCS(-1,0,0);
    std::vector<btVector3> connectionPoints;
    connectionPoints.push_back(btVector3(-CHASSIS_WIDTH+0.3*WHEEL_WIDTH, CONNECTION_HEIGHT, -CHASSIS_LENGTH+WHEEL_RADIUS));
    connectionPoints.push_back(btVector3(CHASSIS_WIDTH-0.3*WHEEL_WIDTH, CONNECTION_HEIGHT, -CHASSIS_LENGTH+WHEEL_RADIUS));
    connectionPoints.push_back(btVector3(-CHASSIS_WIDTH+0.3*WHEEL_WIDTH, CONNECTION_HEIGHT, CHASSIS_LENGTH-WHEEL_RADIUS));
    connectionPoints.push_back(btVector3(CHASSIS_WIDTH-0.3*WHEEL_WIDTH, CONNECTION_HEIGHT, CHASSIS_LENGTH-WHEEL_RADIUS));

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
    setSteering(0);
    mVehicle->setCoordinateSystem(0, 1, 2);
}

Car::~Car() { }

void Car::draw() {
    mShader->use();

    // Draw wheels
    for (int i = 0; i < mVehicle->getNumWheels(); i++) {
        btScalar transform[16];
        mVehicle->getWheelInfo(i).m_worldTransform.getOpenGLMatrix(transform);
        glm::vec3 worldOffset = getWorldOffset();
        glm::mat4 model = btScalar2glmMat4(transform);

        mShader->setMat4("model", model);
        mWheelMesh->draw(*mShader);
    }
}

void Car::createWheel(Core::MeshCreator &meshCreator) {
    // Create circular faces
    int thetaPartitions = 20;
    float theta0 = 0;
    for (int i = 1; i<= thetaPartitions; i++) {
        float theta = glm::radians(360.0*i/thetaPartitions);

        // Positions
        glm::vec3 center(WHEEL_WIDTH, 0, 0);
        glm::vec3 left(WHEEL_WIDTH, WHEEL_RADIUS*glm::sin(theta0), WHEEL_RADIUS*glm::cos(theta0));
        glm::vec3 right(WHEEL_WIDTH, WHEEL_RADIUS*glm::sin(theta), WHEEL_RADIUS*glm::cos(theta));

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

// Public Member Functions
void Car::setup() {
    // Create wheel
    Core::MeshCreator wheelMeshCreator;
    createWheel(wheelMeshCreator);
    mWheelMesh = wheelMeshCreator.create();

    mDrawable = mWheelMesh;
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

void Car::applyEngineForce(double force) {
    for (int i=0; i < mVehicle->getNumWheels(); i++) {
        btWheelInfo &wheel = mVehicle->getWheelInfo(i);
        // 4WD
        mVehicle->applyEngineForce(force, i);
        std::cout << wheel.m_engineForce << std::endl;
    }
}

void Car::setSteering(double steering) {
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
        setPosition(START_POS);
        setRotation(START_ROTATION);
        mRigidBody->setLinearVelocity(btVector3(0,0,0));
        mRigidBody->setAngularVelocity(btVector3(0,0,0));
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
        setSteering(mSteering + WHEEL_TURN_RATE*deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        setSteering(mSteering - WHEEL_TURN_RATE*deltaTime);
    }

    // https://pybullet.org/Bullet/phpBB3/viewtopic.php?t=12045
    mRigidBody->setActivationState(ACTIVE_TAG);
}

void Car::perFrame(double deltaTime) {
    std::ostringstream cpOSS;
    glm::vec3 position = getPosition();
    cpOSS << std::fixed << std::setprecision(5) << "Car Pos: (" << position[0] << ", " << position[1] << ", " << position[2] << ")";
    textRenderer->renderText(cpOSS.str(), 1, glm::vec3(0.5,0.5,0.5));
}
