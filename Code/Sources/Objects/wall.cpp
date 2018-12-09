#include "Objects/wall.hpp"

#include "Core/meshcreator.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>

using namespace Objects;


std::shared_ptr<Core::Mesh> Wall::mMesh;
std::shared_ptr<Core::Drawable> Wall::mDrawable;
std::shared_ptr<Core::Shader> Wall::mShader;


const float WALL_DEPTH = 0.15f;
const float WALL_HEIGHT = 1.3f;

const float TEXTURE_REPEAT_S = 1.0f/3.0f;
const float TEXTURE_REPEAT_T = 1.0f;


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

void Wall::addSegment(Core::MeshCreator &meshCreator, int segNumber, float a, float b, float theta1, float theta2) {
    // Positions
    glm::vec3 leftTopBack = glm::vec3((a+WALL_DEPTH/2)*glm::cos(theta1),  1.0f, (b+WALL_DEPTH/2)*glm::sin(theta1));
    glm::vec3 leftTopFront = glm::vec3((a-WALL_DEPTH/2)*glm::cos(theta1),  1.0f, (b-WALL_DEPTH/2)*glm::sin(theta1));
    glm::vec3 leftBottomBack = glm::vec3((a+WALL_DEPTH/2)*glm::cos(theta1),  0.0f, (b+WALL_DEPTH/2)*glm::sin(theta1));
    glm::vec3 leftBottomFront = glm::vec3((a-WALL_DEPTH/2)*glm::cos(theta1),  0.0f, (b-WALL_DEPTH/2)*glm::sin(theta1));
    glm::vec3 rightTopBack = glm::vec3((a+WALL_DEPTH/2)*glm::cos(theta2),  1.0f, (b+WALL_DEPTH/2)*glm::sin(theta2));
    glm::vec3 rightTopFront = glm::vec3((a-WALL_DEPTH/2)*glm::cos(theta2),  1.0f, (b-WALL_DEPTH/2)*glm::sin(theta2));
    glm::vec3 rightBottomBack = glm::vec3((a+WALL_DEPTH/2)*glm::cos(theta2),  0.0f, (b+WALL_DEPTH/2)*glm::sin(theta2));
    glm::vec3 rightBottomFront = glm::vec3((a-WALL_DEPTH/2)*glm::cos(theta2),  0.0f, (b-WALL_DEPTH/2)*glm::sin(theta2));

    // Front wall, first triangle
    meshCreator.mPositions.push_back(glm::vec3(1,WALL_HEIGHT,1)*leftTopFront);
    meshCreator.mPositions.push_back(glm::vec3(1,WALL_HEIGHT,1)*leftBottomFront);
    meshCreator.mPositions.push_back(glm::vec3(1,WALL_HEIGHT,1)*rightBottomFront);
    meshCreator.mNormals.push_back(glm::vec3(leftTopFront[0], 0, leftTopFront[2]));
    meshCreator.mNormals.push_back(glm::vec3(leftBottomFront[0], 0, leftBottomFront[2]));
    meshCreator.mNormals.push_back(glm::vec3(rightBottomFront[0], 0, rightBottomFront[2]));
    meshCreator.mTexCoords.push_back(glm::vec2(TEXTURE_REPEAT_S*(segNumber%3), TEXTURE_REPEAT_T*leftTopFront[1]));
    meshCreator.mTexCoords.push_back(glm::vec2(TEXTURE_REPEAT_S*(segNumber%3), TEXTURE_REPEAT_T*leftBottomFront[1]));
    meshCreator.mTexCoords.push_back(glm::vec2(TEXTURE_REPEAT_S*(segNumber%3+1), TEXTURE_REPEAT_T*rightBottomFront[1]));

    // Front wall, second triangle
    meshCreator.mPositions.push_back(glm::vec3(1,WALL_HEIGHT,1)*leftTopFront);
    meshCreator.mPositions.push_back(glm::vec3(1,WALL_HEIGHT,1)*rightBottomFront);
    meshCreator.mPositions.push_back(glm::vec3(1,WALL_HEIGHT,1)*rightTopFront);
    meshCreator.mNormals.push_back(glm::vec3(leftTopFront[0], 0, leftTopFront[2]));
    meshCreator.mNormals.push_back(glm::vec3(rightBottomFront[0], 0, rightBottomFront[2]));
    meshCreator.mNormals.push_back(glm::vec3(rightTopFront[0], 0, rightTopFront[2]));
    meshCreator.mTexCoords.push_back(glm::vec2(TEXTURE_REPEAT_S*(segNumber%3), TEXTURE_REPEAT_T*leftTopFront[1]));
    meshCreator.mTexCoords.push_back(glm::vec2(TEXTURE_REPEAT_S*(segNumber%3+1), TEXTURE_REPEAT_T*rightBottomFront[1]));
    meshCreator.mTexCoords.push_back(glm::vec2(TEXTURE_REPEAT_S*(segNumber%3+1), TEXTURE_REPEAT_T*rightTopFront[1]));

    // Back wall, first triangle
    meshCreator.mPositions.push_back(glm::vec3(1,WALL_HEIGHT,1)*leftTopBack);
    meshCreator.mPositions.push_back(glm::vec3(1,WALL_HEIGHT,1)*leftBottomBack);
    meshCreator.mPositions.push_back(glm::vec3(1,WALL_HEIGHT,1)*rightBottomBack);
    meshCreator.mNormals.push_back(glm::vec3(leftTopBack[0], 0, leftTopBack[2]));
    meshCreator.mNormals.push_back(glm::vec3(leftBottomBack[0], 0, leftBottomBack[2]));
    meshCreator.mNormals.push_back(glm::vec3(rightBottomBack[0], 0, rightBottomBack[2]));
    meshCreator.mTexCoords.push_back(glm::vec2(TEXTURE_REPEAT_S*(segNumber%3+1), TEXTURE_REPEAT_T*leftTopBack[1]));
    meshCreator.mTexCoords.push_back(glm::vec2(TEXTURE_REPEAT_S*(segNumber%3+1), TEXTURE_REPEAT_T*leftBottomBack[1]));
    meshCreator.mTexCoords.push_back(glm::vec2(TEXTURE_REPEAT_S*(segNumber%3), TEXTURE_REPEAT_T*rightBottomBack[1]));

    // Back wall, second triangle
    meshCreator.mPositions.push_back(glm::vec3(1,WALL_HEIGHT,1)*leftTopBack);
    meshCreator.mPositions.push_back(glm::vec3(1,WALL_HEIGHT,1)*rightBottomBack);
    meshCreator.mPositions.push_back(glm::vec3(1,WALL_HEIGHT,1)*rightTopBack);
    meshCreator.mNormals.push_back(glm::vec3(leftTopBack[0], 0, leftTopBack[2]));
    meshCreator.mNormals.push_back(glm::vec3(rightBottomBack[0], 0, rightBottomBack[2]));
    meshCreator.mNormals.push_back(glm::vec3(rightTopBack[0], 0, rightTopBack[2]));
    meshCreator.mTexCoords.push_back(glm::vec2(TEXTURE_REPEAT_S*(segNumber%3+1), TEXTURE_REPEAT_T*leftTopBack[1]));
    meshCreator.mTexCoords.push_back(glm::vec2(TEXTURE_REPEAT_S*(segNumber%3), TEXTURE_REPEAT_T*rightBottomBack[1]));
    meshCreator.mTexCoords.push_back(glm::vec2(TEXTURE_REPEAT_S*(segNumber%3), TEXTURE_REPEAT_T*rightTopBack[1]));

    // Top, first triangle
    meshCreator.mPositions.push_back(glm::vec3(1,WALL_HEIGHT,1)*leftTopBack);
    meshCreator.mPositions.push_back(glm::vec3(1,WALL_HEIGHT,1)*leftTopFront);
    meshCreator.mPositions.push_back(glm::vec3(1,WALL_HEIGHT,1)*rightTopFront);
    meshCreator.mNormals.push_back(glm::vec3(leftTopBack[0], 0, leftTopBack[2]));
    meshCreator.mNormals.push_back(glm::vec3(leftTopFront[0], 0, leftTopFront[2]));
    meshCreator.mNormals.push_back(glm::vec3(rightTopFront[0], 0, rightTopFront[2]));
    meshCreator.mTexCoords.push_back(glm::vec2(0, 0));
    meshCreator.mTexCoords.push_back(glm::vec2(0, 0));
    meshCreator.mTexCoords.push_back(glm::vec2(0, 0));

    // Top, second triangle
    meshCreator.mPositions.push_back(glm::vec3(1,WALL_HEIGHT,1)*leftTopBack);
    meshCreator.mPositions.push_back(glm::vec3(1,WALL_HEIGHT,1)*rightTopFront);
    meshCreator.mPositions.push_back(glm::vec3(1,WALL_HEIGHT,1)*rightTopBack);
    meshCreator.mNormals.push_back(glm::vec3(leftTopBack[0], 0, leftTopBack[2]));
    meshCreator.mNormals.push_back(glm::vec3(rightTopFront[0], 0, rightTopFront[2]));
    meshCreator.mNormals.push_back(glm::vec3(rightTopBack[0], 0, rightTopBack[2]));
    meshCreator.mTexCoords.push_back(glm::vec2(0, 0));
    meshCreator.mTexCoords.push_back(glm::vec2(0, 0));
    meshCreator.mTexCoords.push_back(glm::vec2(0, 0));
}

void Wall::setup() {
    // Generate walls
    Core::MeshCreator meshCreator;
    int segments = 70;
    float theta0 = 0;
    for (int i = 0; i <= segments; i++) {
        float theta = glm::radians(360.0)*i/segments;
        // Outer wall
        addSegment(meshCreator, i, trackOuterA, trackOuterB, theta0, theta);
        // Inner wall
        addSegment(meshCreator, i, trackInnerA, trackInnerB, theta0, theta);
        theta0 = theta;
    }

    meshCreator.addTexture(PROJECT_SOURCE_DIR "/Textures/Wall/logo.jpg");

    mMesh = meshCreator.create();
    mDrawable = mMesh;
    mShader = defaultShader;
}