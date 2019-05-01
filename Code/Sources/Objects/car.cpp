#include "Objects/car.hpp"
#include "Components/camera.hpp"
#include "Components/carphysicsbody.hpp"
#include "Components/wheelmeshrenderer.hpp"
#include "Components/particlesystemrenderer.hpp"
#include "Components/meshfilter.hpp"
#include "Components/meshrenderer.hpp"
#include "Components/spotlight.hpp"
#include "Scripts/carscript.hpp"
#include "Scripts/camerascript.hpp"
#include "Utils/meshcreator.hpp"
#include "Utils/transformconversions.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>

#include <iostream>

using namespace Objects;

const float SCALE_FACTOR = 1.0f/400.0f;

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

const float	WHEEL_FRICTION = 1000.0f;
const float SUSPENSION_STIFFNESS = 50.f;
const float SUSPENSION_DAMPING = 4.3f;
const float SUSPENSION_COMPRESSION = 4.4f;
const float SUSPENSION_REST_LENGTH = 0.12f;
const float	ROLL_INFLUENCE = 0.02f;

namespace Objects
{
    std::shared_ptr<Assets::Model> Car::mModel;
    std::shared_ptr<Assets::Material> Car::mMaterial;
    std::shared_ptr<Assets::ParticleSystem> Car::mParticleSystem;

    Car::Car(glm::vec3 position, const Physics::PhysicsEngine &physicsEngine) : Core::GameObject(position)
    {
        // **** SETUP TRANSFORM ****
        mTransform->setScale(glm::vec3(SCALE_FACTOR));

        // **** CREATE COMPONENTS ****
        // Create mesh filter for main chassis
        auto chassisMeshFilter = std::make_shared<Components::MeshFilter>(*this);
        chassisMeshFilter->mMesh = mModel->mMeshes[0];
        addComponent(chassisMeshFilter);

        // Create mesh filter for glass paneling
        auto glassMeshFilter = std::make_shared<Components::MeshFilter>(*this);
        glassMeshFilter->mMesh = mModel->mMeshes[1];
        addComponent(glassMeshFilter);

        // Create car renderer
        auto carMeshRenderer = std::make_shared<Components::MeshRenderer>(*this);
        carMeshRenderer->mMaterial = mMaterial;
        addComponent(carMeshRenderer);

        // Create wheel mesh renderer
        auto wheelMeshRenderer = std::make_shared<Components::WheelMeshRenderer>(*this);
        wheelMeshRenderer->mMaterial = mMaterial;
        wheelMeshRenderer->mWheelMeshes.push_back(mModel->mMeshes[2]);    // Wheel 1
        wheelMeshRenderer->mWheelMeshes.push_back(mModel->mMeshes[3]);    // Wheel 2
        wheelMeshRenderer->mWheelMeshes.push_back(mModel->mMeshes[4]);    // Wheel 3
        wheelMeshRenderer->mWheelMeshes.push_back(mModel->mMeshes[5]);    // Wheel 4
        wheelMeshRenderer->mWheelModelMatrices.reserve(4);
        addComponent(wheelMeshRenderer);

        // Create physics body
        auto carPhysicsBody = std::make_shared<Components::CarPhysicsBody>(*this);
        btTransform localTrans;
        localTrans.setIdentity();
        localTrans.setOrigin(btVector3(0,0.15,0));
        carPhysicsBody->mShape = std::make_unique<btCompoundShape>();
        carPhysicsBody->addChildShape(
            localTrans,
            std::make_shared<btBoxShape>(btVector3(CHASSIS_WIDTH, CHASSIS_HEIGHT, CHASSIS_LENGTH))
        );
        btTransform carTransform;
        carTransform.setIdentity();
        carTransform.setOrigin(Utils::TransformConversions::glmVec32btVector3(position));
        btScalar mass(MASS);
        btVector3 localInertia(0, 0, 0);
        carPhysicsBody->mShape->calculateLocalInertia(mass, localInertia);
        carPhysicsBody->mMotionState = std::make_unique<btDefaultMotionState>(carTransform);
        btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, &(*carPhysicsBody->mMotionState), &(*carPhysicsBody->mShape), localInertia);
        carPhysicsBody->mRigidBody = std::make_unique<btRigidBody>(rbInfo);

        carPhysicsBody->mVehicleRaycaster = std::make_unique<btDefaultVehicleRaycaster>(&(*physicsEngine.mDynamicsWorld));
        btRaycastVehicle::btVehicleTuning tuning;
        carPhysicsBody->mVehicle = std::make_unique<btRaycastVehicle>(tuning, &(*carPhysicsBody->mRigidBody), &(*carPhysicsBody->mVehicleRaycaster));
        carPhysicsBody->mRigidBody->setActivationState(DISABLE_DEACTIVATION);

        btVector3 wheelDirection(0,-1,0);
        btVector3 wheelAxleCS(-1,0,0);
        std::vector<btVector3> connectionPoints;
        connectionPoints.push_back(btVector3(-CHASSIS_WIDTH-WHEEL_OFFSET_SIDE, CONNECTION_HEIGHT, CHASSIS_LENGTH-WHEEL_OFFSET_FRONT));
        connectionPoints.push_back(btVector3(CHASSIS_WIDTH+WHEEL_OFFSET_SIDE, CONNECTION_HEIGHT, CHASSIS_LENGTH-WHEEL_OFFSET_FRONT));
        connectionPoints.push_back(btVector3(-CHASSIS_WIDTH-WHEEL_OFFSET_SIDE, CONNECTION_HEIGHT, -CHASSIS_LENGTH+WHEEL_OFFSET_BACK));
        connectionPoints.push_back(btVector3(CHASSIS_WIDTH+WHEEL_OFFSET_SIDE, CONNECTION_HEIGHT, -CHASSIS_LENGTH+WHEEL_OFFSET_BACK));

        for (auto &connectionPoint : connectionPoints) {
            carPhysicsBody->mVehicle->addWheel(
                connectionPoint,
                wheelDirection,
                wheelAxleCS,
                SUSPENSION_REST_LENGTH,
                WHEEL_RADIUS,
                tuning,
                connectionPoint[2] > 0
            );
        }

        for (int i=0; i < carPhysicsBody->mVehicle->getNumWheels(); i++) {
            btWheelInfo& wheel = carPhysicsBody->mVehicle->getWheelInfo(i);
            wheel.m_suspensionStiffness = SUSPENSION_STIFFNESS;
            wheel.m_wheelsDampingRelaxation = SUSPENSION_DAMPING;
            wheel.m_wheelsDampingCompression = SUSPENSION_COMPRESSION;
            wheel.m_frictionSlip = WHEEL_FRICTION;
            wheel.m_rollInfluence = ROLL_INFLUENCE;
        }

        carPhysicsBody->mVehicle->setCoordinateSystem(0, 1, 2);

        physicsEngine.mDynamicsWorld->addRigidBody(&(*carPhysicsBody->mRigidBody));
        physicsEngine.mDynamicsWorld->addVehicle(&(*carPhysicsBody->mVehicle));

        addComponent<Components::PhysicsBody>(carPhysicsBody);
        addComponent(carPhysicsBody);

        // Create car script
        auto carScript = std::make_shared<Scripts::CarScript>(*this, position);
        addComponent<Components::Script>(carScript);

        // **** CREATE SPOTLIGHTS AND TAILLIGHTS ****
        // Create child gameobjects
        auto leftSpotLightGameObject = std::make_shared<Core::GameObject>(glm::vec3(-0.17/SCALE_FACTOR,0.15/SCALE_FACTOR,0.5/SCALE_FACTOR));
        auto rightSpotLightGameObject = std::make_shared<Core::GameObject>(glm::vec3(0.17/SCALE_FACTOR,0.15/SCALE_FACTOR,0.5/SCALE_FACTOR));
        auto leftTailLightGameObject = std::make_shared<Core::GameObject>(glm::vec3(-0.17/SCALE_FACTOR,0.15/SCALE_FACTOR,-0.5/SCALE_FACTOR));        
        auto rightTailLightGameObject = std::make_shared<Core::GameObject>(glm::vec3(0.17/SCALE_FACTOR,0.15/SCALE_FACTOR,-0.5/SCALE_FACTOR));

        // Rotate lights appropriate amounts
        leftSpotLightGameObject->mTransform->setRotation(glm::angleAxis(glm::radians(5.0f), glm::vec3(1,0,0)));
        rightSpotLightGameObject->mTransform->setRotation(glm::angleAxis(glm::radians(5.0f), glm::vec3(1,0,0)));
        leftTailLightGameObject->mTransform->setRotation(glm::toQuat(glm::yawPitchRoll(glm::radians(180.0f), glm::radians(5.0f), 0.0f)));
        rightTailLightGameObject->mTransform->setRotation(glm::toQuat(glm::yawPitchRoll(glm::radians(180.0f), glm::radians(5.0f), 0.0f)));

        // Add children
        addChild(leftSpotLightGameObject);
        addChild(rightSpotLightGameObject);
        addChild(leftTailLightGameObject);
        addChild(rightTailLightGameObject);
        
        // Create spotlight components
        auto spotLightGameObjects = { leftSpotLightGameObject, rightSpotLightGameObject };
        for (auto spotLightGameObject : spotLightGameObjects) {
            auto spotLight = std::make_shared<Components::SpotLight>(*spotLightGameObject); 
            spotLight->mAmbient = glm::vec3(0.2,0.2,0.2);
            spotLight->mDiffuse = glm::vec3(0.45,0.45,0.45);
            spotLight->mSpecular = glm::vec3(0.55f, 0.55f, 0.55f);
            spotLight->mInnerCutoff = glm::cos(glm::radians(12.5f));
            spotLight->mOuterCutoff = glm::cos(glm::radians(17.5f));
            spotLight->mConstant = 1.0f;
            spotLight->mLinear = 0.09f;
            spotLight->mQuadratic = 0.032f;
            spotLightGameObject->addComponent(spotLight);
        }

        // Create taillight components
        auto tailLightGameObjects = { leftTailLightGameObject, rightTailLightGameObject };
        for (auto tailLightGameObject : tailLightGameObjects) {
            auto tailLight = std::make_shared<Components::SpotLight>(*tailLightGameObject);
            tailLight->mAmbient = glm::vec3(0.23,0.06,0.06);
            tailLight->mDiffuse = glm::vec3(0.35,0.15,0.15);
            tailLight->mSpecular = glm::vec3(0.55f, 0.25f, 0.25f);
            tailLight->mInnerCutoff = glm::cos(glm::radians(12.5f));
            tailLight->mOuterCutoff = glm::cos(glm::radians(17.5f));
            tailLight->mConstant = 1.0f;
            tailLight->mLinear = 0.09f;
            tailLight->mQuadratic = 0.032f;
            tailLightGameObject->addComponent(tailLight);
        }

        // **** CREATE GAMEOBJECT WITH PARTICLE SYSTEM RENDERER ****
        // Create gameobject
        auto psrGameObject = std::make_shared<Core::GameObject>(glm::vec3(0, 0, -0.5));
        addChild(psrGameObject);

        // Create paricle system renderer
        auto particleSystemRenderer = std::make_shared<Components::ParticleSystemRenderer>(*psrGameObject);
        particleSystemRenderer->mParticleSystem = mParticleSystem;
        particleSystemRenderer->setupParticleSystem(211);
        psrGameObject->addComponent(particleSystemRenderer);

        // **** CREATE GAMEOBJECT WITH CAMERA TO FOLLOW CAR ****
        // Create gameobject
        auto cameraGameObject = std::make_shared<Core::GameObject>(glm::vec3(0, 0, 2));
        addChild(cameraGameObject);
        
        // Create camera
        auto camera = std::make_shared<Components::Camera>(*cameraGameObject);
        camera->mFollowTransform = mTransform;
        camera->mProjectionMode = Components::ProjectionMode::PERSPECTIVE;
        camera->mOrthoBoxSize = 3.0f;
        cameraGameObject->addComponent(camera);

        // Create camera script
        auto cameraScript = std::make_shared<Scripts::CameraScript>(*cameraGameObject);
        cameraGameObject->addComponent<Components::Script>(cameraScript);
    }

    Car::~Car()
    {
    }

    void Car::setup(std::shared_ptr<Assets::Shader> geometryShader)
    {
        // ***** CREATE MODEL *****
        // Import model
        auto carModel = std::make_shared<Assets::Model>(
            PROJECT_SOURCE_DIR "/Models/lambo/Lamborghini_Aventador.fbx"
        );

        // Center wheels
        for (int i = 2; i <= 5; i++) {
            carModel->mMeshes[i]->center();
        }

        mModel = carModel;

        // ***** CREATE MATERIAL *****
        auto carMaterial = std::make_shared<Assets::Material>();
        carMaterial->mGeometryShader = geometryShader;
        carMaterial->mAlbedoMap =
            mModel->mAlbedoTextures.size() >= 1 ?
            mModel->mAlbedoTextures[0] :
            nullptr;
        carMaterial->mSpecularMap =
            mModel->mSpecularTextures.size() >= 1 ?
            mModel->mSpecularTextures[0] :
            nullptr;
        carMaterial->mNormalMap =
            mModel->mNormalTextures.size() >= 1 ?
            mModel->mNormalTextures[0] :
            nullptr;
        mMaterial = carMaterial;

        // ***** CREATE PARTICLE SYSTEM *****
        auto particleUpdateShader = std::make_shared<Assets::Shader>(
            PROJECT_SOURCE_DIR "/Shaders/VertexShaders/flames_update.vert",
            PROJECT_SOURCE_DIR "/Shaders/FragmentShaders/flames_update.frag"
        );
        auto particleRenderShader = std::make_shared<Assets::Shader>(
            PROJECT_SOURCE_DIR "/Shaders/VertexShaders/flames_render.vert",
            PROJECT_SOURCE_DIR "/Shaders/GeometryShaders/flames_render.geom",
            PROJECT_SOURCE_DIR "/Shaders/FragmentShaders/flames_render.frag"
        );
        auto particleSystem = std::make_shared<Assets::ParticleSystem>();
        particleSystem->mUpdateShader = particleUpdateShader;
        particleSystem->mRenderShader = particleRenderShader;
        particleSystem->mTextures.push_back(std::make_shared<Assets::Texture>(PROJECT_SOURCE_DIR "/Textures/Particles/flames.tga"));
        particleSystem->mColors.push_back(glm::vec3(0.886, 0.345, 0.133));
        mParticleSystem = particleSystem;
    }
}