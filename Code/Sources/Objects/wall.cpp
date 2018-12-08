#include "Objects/wall.hpp"

#include "Core/meshcreator.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>

using namespace Objects;


std::shared_ptr<Core::Mesh> Wall::mMesh;
std::shared_ptr<Core::Drawable> Wall::mDrawable;
std::shared_ptr<Core::Shader> Wall::mShader;


const float WALL_DEPTH = 0.5f;

std::vector<glm::vec3> segmentPositions {
    // Back
    glm::vec3(-1.0f,  1.0f, -WALL_DEPTH),
    glm::vec3(-1.0f, -1.0f, -WALL_DEPTH),
    glm::vec3(1.0f, -1.0f, -WALL_DEPTH),
    glm::vec3(1.0f, -1.0f, -WALL_DEPTH),
    glm::vec3(1.0f,  1.0f, -WALL_DEPTH),
    glm::vec3(-1.0f,  1.0f, -WALL_DEPTH),

    // Front
    glm::vec3(-1.0f, -1.0f, WALL_DEPTH),
    glm::vec3(-1.0f,  1.0f, WALL_DEPTH),
    glm::vec3(1.0f,  1.0f, WALL_DEPTH),
    glm::vec3(1.0f,  1.0f, WALL_DEPTH),
    glm::vec3(1.0f, -1.0f, WALL_DEPTH),
    glm::vec3(-1.0f, -1.0f, WALL_DEPTH),

    // Top
    glm::vec3(-1.0f,  1.0f, -WALL_DEPTH),
    glm::vec3(1.0f,  1.0f, -WALL_DEPTH),
    glm::vec3(1.0f,  1.0f,  WALL_DEPTH),
    glm::vec3(1.0f,  1.0f,  WALL_DEPTH),
    glm::vec3(-1.0f,  1.0f,  WALL_DEPTH),
    glm::vec3(-1.0f,  1.0f, -WALL_DEPTH),
};

std::vector<glm::vec3> segmentNormals {
    // Back
    glm::vec3(0.0f, 0.0f, -1.0f),
    glm::vec3(0.0f, 0.0f, -1.0f),
    glm::vec3(0.0f, 0.0f, -1.0f),
    glm::vec3(0.0f, 0.0f, -1.0f),
    glm::vec3(0.0f, 0.0f, -1.0f),
    glm::vec3(0.0f, 0.0f, -1.0f),

    // Front
    glm::vec3(0.0f, 0.0f, 1.0f),
    glm::vec3(0.0f, 0.0f, 1.0f),
    glm::vec3(0.0f, 0.0f, 1.0f),
    glm::vec3(0.0f, 0.0f, 1.0f),
    glm::vec3(0.0f, 0.0f, 1.0f),
    glm::vec3(0.0f, 0.0f, 1.0f),

    // Top
    glm::vec3(0.0f, 1.0f, 0.0f),
    glm::vec3(0.0f, 1.0f, 0.0f),
    glm::vec3(0.0f, 1.0f, 0.0f),
    glm::vec3(0.0f, 1.0f, 0.0f),
    glm::vec3(0.0f, 1.0f, 0.0f),
    glm::vec3(0.0f, 1.0f, 0.0f),
};

std::vector<glm::vec2> segmentTexCoords {
    // Back
    glm::vec2(0, 1),
    glm::vec2(0, 0),
    glm::vec2(1, 0),
    glm::vec2(1, 0),
    glm::vec2(1, 1),
    glm::vec2(0, 1),

    // Front
    glm::vec2(0, 1),
    glm::vec2(0, 0),
    glm::vec2(1, 0),
    glm::vec2(1, 0),
    glm::vec2(1, 1),
    glm::vec2(0, 1),

    // Top
    glm::vec2(0, 1),
    glm::vec2(0, 0),
    glm::vec2(1, 0),
    glm::vec2(1, 0),
    glm::vec2(1, 1),
    glm::vec2(0, 1)
};


Wall::Wall() : Core::GameObject(mDrawable, mShader) {
    Core::MeshCreator meshCreator;
    btTriangleMesh *triangleMesh = new btTriangleMesh(false, false);
    for (int i = 0; i < mMesh->vertices.size(); i+=3) {
        triangleMesh->addTriangle(
            glmVec32btVector3(mMesh->vertices[i].Position),
            glmVec32btVector3(mMesh->vertices[i+1].Position),
            glmVec32btVector3(mMesh->vertices[i+2].Position)
        );
    }
    btBvhTriangleMeshShape *wallShape = new btBvhTriangleMeshShape(triangleMesh, true);
    btTransform wallTransform;
    wallTransform.setIdentity();
    wallTransform.setOrigin(btVector3(0, 0.0, 0));
    btScalar mass(0.);
    btVector3 localInertia(0, 0, 0);
    btDefaultMotionState *myMotionState = new btDefaultMotionState(wallTransform);
    btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, wallShape, localInertia);
    mRigidBody = std::make_unique<btRigidBody>(rbInfo);
    dynamicsWorld->addRigidBody(&(*mRigidBody));

}

void Wall::addSegment(Core::MeshCreator &meshCreator, float a, float b, float theta1, float theta2) {
    for (int i = 0; i < segmentPositions.size(); i++) {
        glm::vec3 position;
        if (segmentPositions[i][0] == -1.0f) {
            position = glm::vec3(a*glm::cos(theta1), segmentPositions[i][1], segmentPositions[i][2]+b*glm::sin(theta1));
        }
        else {
            position = position = glm::vec3(a*glm::cos(theta2), segmentPositions[i][1], segmentPositions[i][2]+b*glm::sin(theta2));
        }
        meshCreator.mPositions.push_back(position);
        meshCreator.mNormals.push_back(segmentNormals[i]);
        meshCreator.mTexCoords.push_back(segmentTexCoords[i]);
    }
}

void Wall::setup() {
    // Generate walls
    Core::MeshCreator meshCreator;
    int segments = 40;
    float theta0 = 0;
    for (int i = 0; i <= segments; i++) {
        float theta = glm::radians(360.0)*i/segments;
        // Outer wall
        addSegment(meshCreator, trackOuterA, trackOuterB, theta0, theta);
        // Inner wall
        addSegment(meshCreator, trackInnerA, trackInnerB, theta0, theta);
        theta0 = theta;
    }

    meshCreator.addTexture(PROJECT_SOURCE_DIR "/Textures/Wall/brick.jpg");

    mMesh = meshCreator.create();
    mDrawable = mMesh;
    mShader = defaultShader;
}