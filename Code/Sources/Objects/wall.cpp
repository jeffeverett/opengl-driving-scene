#include "Objects/wall.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>

using namespace Objects;


std::shared_ptr<Core::Drawable> Wall::mDrawable;
std::shared_ptr<Core::Shader> Wall::mShader;
std::vector<Core::Texture> Wall::mTextures;

std::vector<glm::vec3> Wall::mPositions;
std::vector<glm::vec3> Wall::mNormals;
std::vector<glm::vec2> Wall::mTexCoords;


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
    btBoxShape *wallShape = new btBoxShape(btVector3(btScalar(1.), btScalar(1.), btScalar(1.)));
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

void Wall::addSegment(float a, float b, float theta1, float theta2) {
    for (int i = 0; i < segmentPositions.size(); i++) {
        glm::vec3 position;
        if (segmentPositions[i][0] == -1.0f) {
            position = glm::vec3(a*glm::cos(theta1), segmentPositions[i][1], segmentPositions[i][2]+b*glm::sin(theta1));
        }
        else {
            position = position = glm::vec3(a*glm::cos(theta2), segmentPositions[i][1], segmentPositions[i][2]+b*glm::sin(theta2));
        }
        mPositions.push_back(position);
        mNormals.push_back(segmentNormals[i]);
        mTexCoords.push_back(segmentTexCoords[i]);
    }
}

void Wall::setup() {
    // Generate walls
    int segments = 40;
    float theta0 = 0;
    for (int i = 0; i <= segments; i++) {
        float theta = glm::radians(360.0)*i/segments;
        // Outer wall
        addSegment(trackOuterA, trackOuterB, theta0, theta);
        // Inner wall
        addSegment(trackInnerA, trackInnerB, theta0, theta);
        theta0 = theta;
    }

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    int width, height, nrChannels;
    std::string texturePath = PROJECT_SOURCE_DIR "/Textures/Wall/brick.jpg";
    unsigned char *data = stbi_load(texturePath.c_str(), &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << texturePath << std::endl;
        stbi_image_free(data);
    }
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

    mTextures.push_back(Core::Texture { textureID, "texture_diffuse", texturePath });
    mDrawable = std::make_shared<Core::Mesh>(mPositions, mNormals, mTexCoords, mTextures);
    mShader = defaultShader;
}