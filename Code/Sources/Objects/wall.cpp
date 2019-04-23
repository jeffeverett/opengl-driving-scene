#include "Objects/wall.hpp"
#include "Components/physicsbody.hpp"
#include "Components/meshfilter.hpp"
#include "Components/meshrenderer.hpp"
#include "Utils/meshcreator.hpp"
#include "Utils/transformconversions.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>

using namespace Objects;

const int WALL_SEGMENTS = 70;

const float WALL_DEPTH = 0.15f;
const float WALL_HEIGHT = 1.3f;

const float TEXTURE_REPEAT_S = 1.0f/3.0f;
const float TEXTURE_REPEAT_T = 1.0f;


namespace Objects
{
    std::shared_ptr<Assets::Material> Wall::mMaterial;
    std::shared_ptr<Assets::Mesh> Wall::mMesh;
    std::shared_ptr<btTriangleMesh> Wall::mColliderMesh;

    Wall::Wall(glm::vec3 position, const Physics::PhysicsEngine &physicsEngine) : Core::GameObject(position)
    {
        // **** CREATE COMPONENTS ***
        // Create mesh filter
        auto meshFilter = std::make_shared<Components::MeshFilter>(*this);
        meshFilter->mMesh = mMesh;
        addComponent(meshFilter);

        // Create mesh renderer
        auto meshRenderer = std::make_shared<Components::MeshRenderer>(*this);
        meshRenderer->mMaterial = mMaterial;
        addComponent(meshRenderer);

        // Create physics body
        auto physicsBody = std::make_shared<Components::PhysicsBody>(*this);
        physicsBody->mShape =  std::make_unique<btBvhTriangleMeshShape>(&(*mColliderMesh), true);
        btTransform wallTransform;
        wallTransform.setIdentity();
        btScalar mass(0.0f);
        btVector3 localInertia(0, 0, 0);
        physicsBody->mMotionState = std::make_unique<btDefaultMotionState>(wallTransform);
        btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, &(*physicsBody->mMotionState), &(*physicsBody->mShape), localInertia);
        physicsBody->mRigidBody = std::make_unique<btRigidBody>(rbInfo);
        physicsEngine.mDynamicsWorld->addRigidBody(&(*physicsBody->mRigidBody));
        addComponent(physicsBody);
    }

    void Wall::setup(std::shared_ptr<Assets::Shader> geometryShader, float trackInnerA, float trackInnerB, float trackOuterA, float trackOuterB)
    {
        // ***** CREATE MATERIAL *****
        auto wallMaterial = std::make_shared<Assets::Material>();
        wallMaterial->mGeometryShader = geometryShader;
        wallMaterial->mAlbedoMap = std::make_shared<Assets::Texture>(
            PROJECT_SOURCE_DIR "/Textures/Wall/logo.jpg", true
        );
        mMaterial = wallMaterial;
        
        // ***** CREATE MESH *****
        Utils::MeshCreator wallMeshCreator;
        float theta0 = 0;
        for (int i = 0; i <= WALL_SEGMENTS; i++) {
            float theta = glm::radians(360.0f)*i/WALL_SEGMENTS;
            float firstS = TEXTURE_REPEAT_S*(i%3);
            float secondS = TEXTURE_REPEAT_S*(i%3+1);
            // Outer wall
            wallMeshCreator.addEllipticalSegment(firstS, secondS, trackOuterA, trackOuterB, theta0, theta, WALL_HEIGHT, WALL_DEPTH);
            // Inner wall
            wallMeshCreator.addEllipticalSegment(firstS, secondS, trackInnerA, trackInnerB, theta0, theta, WALL_HEIGHT, WALL_DEPTH);
            theta0 = theta;
        }
        mMesh = wallMeshCreator.create();

        // ***** CREATE COLLIDER MESH *****
        mColliderMesh = std::make_shared<btTriangleMesh>(false, false);
        for (int i = 0; i < mMesh->mVertices.size(); i+=3) {
            mColliderMesh->addTriangle(
                Utils::TransformConversions::glmVec32btVector3(mMesh->mVertices[i].Position),
                Utils::TransformConversions::glmVec32btVector3(mMesh->mVertices[i+1].Position),
                Utils::TransformConversions::glmVec32btVector3(mMesh->mVertices[i+2].Position)
            );
        }
    }
}