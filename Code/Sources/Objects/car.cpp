#include "Objects/car.hpp"
#include "Components/camera.hpp"
#include "Components/carphysicsbody.hpp"
#include "Components/meshfilter.hpp"
#include "Components/meshrenderer.hpp"
#include "Components/spotlight.hpp"
#include "Utils/meshcreator.hpp"
#include "Utils/transformconversions.hpp"

using namespace Objects;

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

namespace Objects
{
    std::shared_ptr<Assets::Model> Car::mModel;
    std::shared_ptr<Assets::Material> Car::mMaterial;

    Car::Car(glm::vec3 position, const Physics::PhysicsEngine &physicsEngine) : Core::GameObject(position)
    {
        // **** SETUP TRANSFORM ****
        mTransform->scale(glm::vec3(SCALE_FACTOR));

        // **** CREATE COMPONENTS ****
        // Create mesh filter for main chassis
        auto chassisMeshFilter = std::make_shared<Components::MeshFilter>(*this);
        chassisMeshFilter->mMesh = mModel->mMeshes[0];
        addComponent(chassisMeshFilter);

        // Create mesh filter for glass paneling
        auto glassMeshFilter = std::make_shared<Components::MeshFilter>(*this);
        glassMeshFilter->mMesh = mModel->mMeshes[1];
        addComponent(glassMeshFilter);

        // Create mesh renderer
        auto carMeshRenderer = std::make_shared<Components::MeshRenderer>(*this);
        carMeshRenderer->mMaterial = mMaterial;
        addComponent(carMeshRenderer);

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
        addComponent(carPhysicsBody);

        // **** CREATE SPOTLIGHTS AND TAILLIGHTS ****
        // Create child gameobjects
        auto leftSpotLightGameObject = std::make_shared<Core::GameObject>(glm::vec3(1/SCALE_FACTOR)*glm::vec3(-0.17,0.15,0.5));
        auto rightSpotLightGameObject = std::make_shared<Core::GameObject>(glm::vec3(1/SCALE_FACTOR)*glm::vec3(0.17,0.15,0.5));
        auto leftTailLightGameObject = std::make_shared<Core::GameObject>(glm::vec3(1/SCALE_FACTOR)*glm::vec3(-0.17,0.15,-0.5));        
        auto rightTailLightGameObject = std::make_shared<Core::GameObject>(glm::vec3(1/SCALE_FACTOR)*glm::vec3(0.17,0.15,-0.5));

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
            spotLight->mDiffuse = glm::vec3(0.75,0.75,0.75);
            spotLight->mSpecular = 1.0f;
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
            auto tailLight = std::make_shared<Components::SpotLight>(*leftSpotLightGameObject);
            tailLight->mAmbient = glm::vec3(0.23,0.06,0.06);
            tailLight->mDiffuse = glm::vec3(0.75,0.15,0.15);
            tailLight->mSpecular = 1.0f;
            tailLight->mInnerCutoff = glm::cos(glm::radians(12.5f));
            tailLight->mOuterCutoff = glm::cos(glm::radians(17.5f));
            tailLight->mConstant = 1.0f;
            tailLight->mLinear = 0.09f;
            tailLight->mQuadratic = 0.032f;
            tailLightGameObject->addComponent(tailLight);
        }

        // **** CREATE GAMEOBJECT WITH CAMERA TO FOLLOW CAR ****
        // Create gameobject
        auto cameraGameObject = std::make_shared<Core::GameObject>(glm::vec3(0, 0, -2));
        addChild(cameraGameObject);
        
        // Create camera
        auto camera = std::make_shared<Components::Camera>(*cameraGameObject);
        camera->mProjectionMode = Components::ProjectionMode::ORTHO;
        camera->mOrthoBoxSize = 3.0f;
        cameraGameObject->addComponent(camera);
    }

    Car::~Car()
    {
    }

    void Car::setup(std::shared_ptr<Assets::Shader> geometryShader)
    {
        // ***** CREATE MODEL *****
        auto carModel = std::make_shared<Assets::Model>(
            PROJECT_SOURCE_DIR "/Models/lambo/Lamborghini_Aventador.fbx"
        );
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

        // ***** CREATE WHEEL MESHES *****

    }
}