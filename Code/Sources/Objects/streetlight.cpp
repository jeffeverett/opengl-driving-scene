#include "Objects/streetlight.hpp"
#include "Components/physicsbody.hpp"
#include "Components/meshfilter.hpp"
#include "Components/meshrenderer.hpp"
#include "Components/pointlight.hpp"
#include "Utils/meshcreator.hpp"

using namespace Objects;

const float RADIUS = 0.04f;
const float HEIGHT = 2.5f;
const float ROTATION = glm::radians(150.0f);
const float POST_TEXTURE_SCALE_S = 1;
const float POST_TEXTURE_SCALE_T = 4;

const float A = 0.25f;
const float X_MIN = -0.25f;
const float X_MAX = 0.85f;
const float C = HEIGHT + A*X_MIN*X_MIN;

float poleX0 = X_MIN;
float poleY0 = -A*X_MIN*X_MIN+C;

std::shared_ptr<Assets::Mesh> Streetlight::mPostMesh;
std::shared_ptr<Assets::Material> Streetlight::mPostMaterial;

std::shared_ptr<Assets::Mesh> Streetlight::mBulbMesh;
std::shared_ptr<Assets::Material> Streetlight::mBulbMaterial;


Streetlight::Streetlight(glm::vec3 position, float theta, bool onLeft) : Core::GameObject(position)
{
    if (onLeft) {
        mTransform->setScale(glm::vec3(1, 1, 1));
    }
    else {
        mTransform->setScale(glm::vec3(-1, 1, 1));
    }
    mTransform->setRotation(glm::angleAxis(theta, glm::vec3(0.0f, 1.0f, 0.0f)));

    // **** CREATE COMPONENTS ***
    // Create mesh filter for post
    auto postMeshFilter = std::make_shared<Components::MeshFilter>(*this);
    postMeshFilter->mMesh = mPostMesh;
    addComponent(postMeshFilter);

    // Create mesh renderer for post
    auto postMeshRenderer = std::make_shared<Components::MeshRenderer>(*this);
    postMeshRenderer->mMaterial = mPostMaterial;
    addComponent(postMeshRenderer);

    // Create physics body
    auto physicsBody = std::make_shared<Components::PhysicsBody>(*this);
    physicsBody->mShape = std::make_unique<btCylinderShape>(btVector3(RADIUS, HEIGHT, RADIUS));
    btTransform streetlightTransform;
    streetlightTransform.setIdentity();
    streetlightTransform.setOrigin(btVector3(position[0], 0, position[2]));
    btScalar mass(0.0f);
    btVector3 localInertia(0, 0, 0);
    physicsBody->mMotionState = std::make_unique<btDefaultMotionState>(streetlightTransform);
    btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, &(*physicsBody->mMotionState), &(*physicsBody->mShape), localInertia);
    physicsBody->mRigidBody = std::make_unique<btRigidBody>(rbInfo);
    addComponent(physicsBody);

    // **** CREATE BULB ****
    // Create bulb game object
    auto bulbGameObject = std::make_shared<Core::GameObject>();
    addChild(bulbGameObject);

    // Create mesh filter for bulb
    auto bulbMeshFilter = std::make_shared<Components::MeshFilter>(*bulbGameObject);
    bulbMeshFilter->mMesh = mBulbMesh;
    bulbGameObject->addComponent(bulbMeshFilter);

    // Create mesh renderer for bulb
    auto bulbMeshRenderer = std::make_shared<Components::MeshRenderer>(*bulbGameObject);
    bulbMeshRenderer->mMaterial = mBulbMaterial;
    bulbGameObject->addComponent(bulbMeshRenderer);

    // Create point light
    auto pointLight = std::make_shared<Components::PointLight>(*bulbGameObject);
    pointLight->mAmbient = glm::vec3(0.25f, 0.25f, 0.25f);
    pointLight->mDiffuse = glm::vec3(0.60f, 0.60f, 0.60f);
    pointLight->mSpecular = glm::vec3(1.0f, 1.0f, 1.0f);
    pointLight->mConstant = 1.0f;
    pointLight->mLinear = 0.09f;
    pointLight->mQuadratic = 0.032f;
    bulbGameObject->addComponent(pointLight);
}

Streetlight::~Streetlight()
{
}

void Streetlight::setup(std::shared_ptr<Assets::Shader> geometryShader)
{
    // ***** CREATE POST MATERIAL *****
    auto postMaterial = std::make_shared<Assets::Material>();
    postMaterial->mGeometryShader = geometryShader;
    postMaterial->mAlbedoMap = std::make_shared<Assets::Texture>(
        PROJECT_SOURCE_DIR "/Textures/Streetlight/metal.jpg"
    );
    mPostMaterial = postMaterial;

    // ***** CREATE BULB MATERIAL *****
    auto bulbMaterial = std::make_shared<Assets::Material>();
    bulbMaterial->mGeometryShader = geometryShader;
    bulbMaterial->mAlbedoMap = std::make_shared<Assets::Texture>(
        PROJECT_SOURCE_DIR "/Textures/Streetlight/glass.jpg"
    );
    mBulbMaterial = bulbMaterial;

    // ***** CREATE POST MESH *****
    // Create mesh creator
    Utils::MeshCreator postMeshCreator;

    // Create post geometry
    int partitions = 10;
    postMeshCreator.mTextureScaleS = POST_TEXTURE_SCALE_S;
    postMeshCreator.mTextureScaleT = POST_TEXTURE_SCALE_T;
    for (int i = 1; i <= partitions; i++) {
        postMeshCreator.addOpenCylinder(HEIGHT/partitions, (i-1)*HEIGHT/partitions, i*HEIGHT/partitions, RADIUS);
    }

    // Create curved head, following parabolic curve
    // y = -Ax^2+C, where C is the constant s.t. y(X_MIN) = HEIGHT
    partitions = 15;
    for (int i = 0; i <= partitions; i++) {
        float x = X_MIN+i*(X_MAX-X_MIN)/partitions;
        float y = -A*x*x+C;

        postMeshCreator.addRotatedOpenCylinder(poleY0, y, RADIUS, poleX0-X_MIN, x-X_MIN, ROTATION*(i-1)/partitions, ROTATION*i/partitions);

        poleX0 = x;
        poleY0 = y;
    }

    // Create bulb guard shape
    postMeshCreator.addSphere(90.0f, glm::vec3(poleX0-X_MIN, poleY0, 0), 0.2, glm::radians(180.0f)-ROTATION);

    // Convert into mesh
    mPostMesh = postMeshCreator.create();

    // **** CREATE BULB MESH ****
    Utils::MeshCreator bulbMeshCreator;
    bulbMeshCreator.addSphere(180.0f, glm::vec3(poleX0-X_MIN, poleY0, 0), 0.18, 0);
    mBulbMesh = bulbMeshCreator.create();
}