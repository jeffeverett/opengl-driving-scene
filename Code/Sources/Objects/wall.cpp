#include "Objects/wall.hpp"

using namespace Objects;


std::shared_ptr<Utils::Drawable> Wall::mDrawable;
std::shared_ptr<Utils::Shader> Wall::mShader;
std::vector<Utils::Texture> Wall::mTextures;

std::vector<glm::vec3> Wall::mPositions {
    // Back
    glm::vec3(-1.0f,  1.0f, -1.0f),
    glm::vec3(-1.0f, -1.0f, -1.0f),
    glm::vec3(1.0f, -1.0f, -1.0f),
    glm::vec3(1.0f, -1.0f, -1.0f),
    glm::vec3(1.0f,  1.0f, -1.0f),
    glm::vec3(-1.0f,  1.0f, -1.0f),

    // Left
    glm::vec3(-1.0f, -1.0f,  1.0f),
    glm::vec3(-1.0f, -1.0f, -1.0f),
    glm::vec3(-1.0f,  1.0f, -1.0f),
    glm::vec3(-1.0f,  1.0f, -1.0f),
    glm::vec3(-1.0f,  1.0f,  1.0f),
    glm::vec3(-1.0f, -1.0f,  1.0f),

    // Right
    glm::vec3(1.0f, -1.0f, -1.0f),
    glm::vec3(1.0f, -1.0f,  1.0f),
    glm::vec3(1.0f,  1.0f,  1.0f),
    glm::vec3(1.0f,  1.0f,  1.0f),
    glm::vec3(1.0f,  1.0f, -1.0f),
    glm::vec3(1.0f, -1.0f, -1.0f),

    // Front
    glm::vec3(-1.0f, -1.0f,  1.0f),
    glm::vec3(-1.0f,  1.0f,  1.0f),
    glm::vec3(1.0f,  1.0f,  1.0f),
    glm::vec3(1.0f,  1.0f,  1.0f),
    glm::vec3(1.0f, -1.0f,  1.0f),
    glm::vec3(-1.0f, -1.0f,  1.0f),

    // Top
    glm::vec3(-1.0f,  1.0f, -1.0f),
    glm::vec3(1.0f,  1.0f, -1.0f),
    glm::vec3(1.0f,  1.0f,  1.0f),
    glm::vec3(1.0f,  1.0f,  1.0f),
    glm::vec3(-1.0f,  1.0f,  1.0f),
    glm::vec3(-1.0f,  1.0f, -1.0f),

    // Bottom
    glm::vec3(-1.0f, -1.0f, -1.0f),
    glm::vec3(-1.0f, -1.0f,  1.0f),
    glm::vec3(1.0f, -1.0f, -1.0f),
    glm::vec3(1.0f, -1.0f, -1.0f),
    glm::vec3(-1.0f, -1.0f,  1.0f),
    glm::vec3(1.0f, -1.0f,  1.0f)
};

std::vector<glm::vec3> Wall::mNormals {
    // Back
    glm::vec3(0.0f, 0.0f, -1.0f),
    glm::vec3(0.0f, 0.0f, -1.0f),
    glm::vec3(0.0f, 0.0f, -1.0f),
    glm::vec3(0.0f, 0.0f, -1.0f),
    glm::vec3(0.0f, 0.0f, -1.0f),
    glm::vec3(0.0f, 0.0f, -1.0f),

    // Left
    glm::vec3(-1.0f, 0.0f, 0.0f),
    glm::vec3(-1.0f, 0.0f, 0.0f),
    glm::vec3(-1.0f, 0.0f, 0.0f),
    glm::vec3(-1.0f, 0.0f, 0.0f),
    glm::vec3(-1.0f, 0.0f, 0.0f),
    glm::vec3(-1.0f, 0.0f, 0.0f),

    // Right
    glm::vec3(1.0f, 0.0f, 0.0f),
    glm::vec3(1.0f, 0.0f, 0.0f),
    glm::vec3(1.0f, 0.0f, 0.0f),
    glm::vec3(1.0f, 0.0f, 0.0f),
    glm::vec3(1.0f, 0.0f, 0.0f),
    glm::vec3(1.0f, 0.0f, 0.0f),

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

    // Bottom
    glm::vec3(0.0f, -1.0f, 0.0f),
    glm::vec3(0.0f, -1.0f, 0.0f),
    glm::vec3(0.0f, -1.0f, 0.0f),
    glm::vec3(0.0f, -1.0f, 0.0f),
    glm::vec3(0.0f, -1.0f, 0.0f),
    glm::vec3(0.0f, -1.0f, 0.0f)
};

std::vector<glm::vec2> Wall::mTexCoords {
    // Back
    glm::vec2(0, 1),
    glm::vec2(0, 0),
    glm::vec2(1, 0),
    glm::vec2(1, 0),
    glm::vec2(1, 1),
    glm::vec2(0, 1),

    // Left
    glm::vec2(0, 1),
    glm::vec2(0, 0),
    glm::vec2(1, 0),
    glm::vec2(1, 0),
    glm::vec2(1, 1),
    glm::vec2(0, 1),

    // Right
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
    glm::vec2(0, 1),

    // Bottom
    glm::vec2(0, 1),
    glm::vec2(0, 0),
    glm::vec2(1, 0),
    glm::vec2(1, 0),
    glm::vec2(1, 1),
    glm::vec2(0, 1)
};


Wall::Wall() : Utils::GameObject(mDrawable, mShader) {
    // Call setup before constructor

    btBoxShape *wallShape = new btBoxShape(btVector3(btScalar(1.), btScalar(1.), btScalar(1.)));
    btTransform wallTransform;
    wallTransform.setIdentity();
    wallTransform.setOrigin(btVector3(0, 0.0, -10));
    btScalar mass(0.);
    btVector3 localInertia(0, 0, 0);
    btDefaultMotionState *myMotionState = new btDefaultMotionState(wallTransform);
    btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, wallShape, localInertia);
    mRigidBody = std::make_unique<btRigidBody>(rbInfo);
    dynamicsWorld->addRigidBody(&(*mRigidBody));

}

void Wall::setup() {
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

    mTextures.push_back(Utils::Texture { textureID, "texture_diffuse", texturePath });
    mDrawable = std::make_shared<Utils::Mesh>(mPositions, mNormals, mTexCoords, mTextures);
    mShader = defaultShader;
}