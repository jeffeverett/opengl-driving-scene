#include "Objects/streetlight.hpp"

using namespace Objects;

std::shared_ptr<Utils::Drawable> Streetlight::mDrawable;
std::shared_ptr<Utils::Shader> Streetlight::mShader;
std::vector<Utils::Texture> Streetlight::mTextures;

std::vector<glm::vec3> Streetlight::mPositions;
std::vector<glm::vec3> Streetlight::mNormals;
std::vector<glm::vec2> Streetlight::mTexCoords;

Streetlight::Streetlight() : Utils::GameObject(mDrawable, mShader) {
    // Call setup before constructor

    btBoxShape *streetlightShape = new btBoxShape(btVector3(btScalar(1.), btScalar(1.), btScalar(1.)));
    btTransform streetlightTransform;
    streetlightTransform.setIdentity();
    streetlightTransform.setOrigin(btVector3(0, 0.0, 0));
    btScalar mass(0.);
    btVector3 localInertia(0, 0, 0);
    btDefaultMotionState *myMotionState = new btDefaultMotionState(streetlightTransform);
    btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, streetlightShape, localInertia);
    mRigidBody = std::make_unique<btRigidBody>(rbInfo);
    dynamicsWorld->addRigidBody(&(*mRigidBody));

}

void Streetlight::createCircle(float height) {
    int partitions = 10;
    float theta = 360;
    for (int i = 0; i <= partitions; i++) {
        glm::vec3 position = glm::vec3(glm::cos(glm::radians(i*theta/partitions)), 0, glm::sin(glm::radians(i*theta/partitions)));
        mPositions.push_back(position);
        mNormals.push_back(glm::vec3(position[0], 0, position[2]));
        mTexCoords.push_back(glm::vec2(height, i/partitions));
    }
}

void Streetlight::setup() {
    // Create main shaft
    int partitions = 10;
    float shaftHeight = 3;
    for (int i = 0; i <= partitions; i++) {
        createCircle(i*shaftHeight/partitions);
    }

    // Create curved head, following parabolic curve
    // y = -x^2
    partitions = 15;
    float xMin = -0.25;
    float xMax = 1;
    float baseHeight = shaftHeight + xMin*xMin;
    for (int i = 0; i <= partitions; i++) {
        float x = 9*(xMax-xMin)/partitions;
        float y = -x*x;
        createCircle(i*(baseHeight+y)/partitions);
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

    mTextures.push_back(Utils::Texture { textureID, "texture_diffuse", texturePath });
    mDrawable = std::make_shared<Utils::Mesh>(mPositions, mNormals, mTexCoords, mTextures);
    mShader = defaultShader;
}