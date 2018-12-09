// Local Headers
#include "Core/gameobject.hpp"

// System Headers
#include <glm/gtc/matrix_transform.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>

// Define Namespace
namespace Core
{
    glm::vec3 GameObject::getPosition() {
        btVector3 origin = mRigidBody->getWorldTransform().getOrigin();
        return btVector32glmVec3(origin)+getWorldOffset();
    }

    void GameObject::setPosition(glm::vec3 position) {
        mRigidBody->getWorldTransform().setOrigin(glmVec32btVector3(position));
    }

    void GameObject::setRotation(float theta) {
        mRigidBody->getWorldTransform().setRotation(btQuaternion(btVector3(0,1,0), glm::radians(theta)));
    }

    void GameObject::setRenderRotation(float theta) {
        mRenderRotation = glm::eulerAngleY(theta);
    }

    void GameObject::setOffset(glm::vec3 offset) {
        mOffset = offset;
    }

    float GameObject::getYaw() {
        btMatrix3x3 rotationMatrix = mRigidBody->getWorldTransform().getBasis();
        btScalar yaw =0, pitch = 0, roll = 0;
        rotationMatrix.getEulerZYX(yaw, pitch, roll);
#ifdef DEBUG
        std::cout << "Pitch: " << pitch << std::endl;
#endif
        return pitch;
    }

    glm::vec3 GameObject::getWorldOffset() {
        btMatrix3x3 rotationMatrix = mRigidBody->getWorldTransform().getBasis();
        btVector3 worldOffset = rotationMatrix*glmVec32btVector3(mOffset);
        return btVector32glmVec3(worldOffset);
    }

    glm::vec3 GameObject::getWorldForward() {
        btMatrix3x3 rotationMatrix = mRigidBody->getWorldTransform().getBasis();
        btVector3 worldOffset = rotationMatrix*btVector3(0,0,1);
        return btVector32glmVec3(worldOffset);
    }

    glm::vec3 GameObject::getWorldDown() {
        btMatrix3x3 rotationMatrix = mRigidBody->getWorldTransform().getBasis();
        btVector3 worldOffset = rotationMatrix*btVector3(0,-1,0);
        return btVector32glmVec3(worldOffset);
    }

    glm::vec3 GameObject::getWorldOffset(glm::vec3 localOffset) {
        btMatrix3x3 rotationMatrix = mRigidBody->getWorldTransform().getBasis();
        btVector3 worldOffset = rotationMatrix*glmVec32btVector3(localOffset);
        glm::vec3 offset = btVector32glmVec3(worldOffset)*mScale;
        return glm::mat3(mRenderRotation)*offset;
    }

    void GameObject::translate(glm::vec3 translationVector) {
        btVector3 origin = mRigidBody->getWorldTransform().getOrigin();
        origin += glmVec32btVector3(translationVector);
        mRigidBody->getWorldTransform().setOrigin(origin);
    }

    void GameObject::applyForce(glm::vec3 vec) {
        btVector3 force = glmVec32btVector3(vec);
        mRigidBody->applyCentralForce(force);
    }

    void GameObject::scale(glm::vec3 scaleVector) {
        mScale *= scaleVector;
    }

    std::shared_ptr<Core::Shader> GameObject::getShader() {
        return mShader;
    }

    void GameObject::draw() {
        // Construct model matrix
        btScalar transform[16];
        mRigidBody->getWorldTransform().getOpenGLMatrix(transform);
        glm::vec3 worldOffset = getWorldOffset();
        transform[12] += worldOffset[0];
        transform[13] += worldOffset[1];
        transform[14] += worldOffset[2];
        glm::mat4 model = btScalar2glmMat4(transform);
        model = model*mRenderRotation;
        model = glm::scale(model, mScale);

        mShader->use();
        mShader->setMat4("model", model);
        mDrawable->draw(*mShader);
    }

    // https://stackoverflow.com/questions/22002077/getting-bullet-physics-transform-matrix-for-opengl
    glm::mat4 GameObject::btScalar2glmMat4(btScalar* matrix) {
        return glm::mat4(
                matrix[0], matrix[1], matrix[2], matrix[3],
                matrix[4], matrix[5], matrix[6], matrix[7],
                matrix[8], matrix[9], matrix[10], matrix[11],
                matrix[12], matrix[13], matrix[14], matrix[15]);
    }

    glm::vec3 GameObject::btVector32glmVec3(btVector3 vec) {
        return glm::vec3(vec[0], vec[1], vec[2]);
    }

    btVector3 GameObject::glmVec32btVector3(glm::vec3 vec) {
        return btVector3(vec[0], vec[1], vec[2]);
    }
}
