#include "Objects/streetlight.hpp"

using namespace Objects;

std::shared_ptr<Utils::Drawable> Streetlight::mDrawable;
std::shared_ptr<Utils::Shader> Streetlight::mShader;
std::vector<Utils::Texture> Streetlight::mTextures;

std::vector<glm::vec3> Streetlight::mPositions;
std::vector<glm::vec3> Streetlight::mNormals;
std::vector<glm::vec2> Streetlight::mTexCoords;

const float RADIUS = 0.04f;
const float HEIGHT = 5.5f;

Streetlight::Streetlight() : Utils::GameObject(mDrawable, mShader) {
    // Call setup before constructor

    btCylinderShape *streetlightShape = new btCylinderShape(btVector3(RADIUS, HEIGHT, RADIUS));
    btTransform streetlightTransform;
    streetlightTransform.setIdentity();
    streetlightTransform.setOrigin(btVector3(0, -4, 0));
    btScalar mass(0.);
    btVector3 localInertia(0, 0, 0);
    btDefaultMotionState *myMotionState = new btDefaultMotionState(streetlightTransform);
    btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, streetlightShape, localInertia);
    mRigidBody = std::make_unique<btRigidBody>(rbInfo);
    dynamicsWorld->addRigidBody(&(*mRigidBody));

}

void Streetlight::createOpenCylinder(float height1, float height2, float radius) {
    int partitions = 10;
    float theta = 360;
    for (int i = 1; i <= partitions; i++) {
        // Positions
        glm::vec3 leftTop = glm::vec3(radius*glm::cos(glm::radians((i-1)*theta/partitions)), height2, radius*glm::sin(glm::radians((i-1)*theta/partitions)));
        glm::vec3 leftBottom = glm::vec3(radius*glm::cos(glm::radians((i-1)*theta/partitions)), height1, radius*glm::sin(glm::radians((i-1)*theta/partitions)));
        glm::vec3 rightTop = glm::vec3(radius*glm::cos(glm::radians(i*theta/partitions)), height2, radius*glm::sin(glm::radians(i*theta/partitions)));
        glm::vec3 rightBottom = glm::vec3(radius*glm::cos(glm::radians(i*theta/partitions)), height1, radius*glm::sin(glm::radians(i*theta/partitions)));

        // Create first triangle
        mPositions.push_back(leftTop);
        mPositions.push_back(leftBottom);
        mPositions.push_back(rightBottom);
        mNormals.push_back(glm::vec3(leftTop[0], 0, leftTop[2]));
        mNormals.push_back(glm::vec3(leftBottom[0], 0, leftBottom[2]));
        mNormals.push_back(glm::vec3(rightBottom[0], 0, rightBottom[2]));
        mTexCoords.push_back(glm::vec2((i-1)/partitions, height2));
        mTexCoords.push_back(glm::vec2((i-1)/partitions, height1));
        mTexCoords.push_back(glm::vec2(i/partitions, height1));

        // Create second triangle
        mPositions.push_back(leftTop);
        mPositions.push_back(rightBottom);
        mPositions.push_back(rightTop);
        mNormals.push_back(glm::vec3(leftTop[0], 0, leftTop[2]));
        mNormals.push_back(glm::vec3(rightBottom[0], 0, rightBottom[2]));
        mNormals.push_back(glm::vec3(rightTop[0], 0, rightTop[2]));
        mTexCoords.push_back(glm::vec2((i-1)/partitions, height2));
        mTexCoords.push_back(glm::vec2(i/partitions, height1));
        mTexCoords.push_back(glm::vec2(i/partitions, height2));
    }
}

void Streetlight::setup() {
    // Create main shaft
    int partitions = 10;
    for (int i = 1; i <= partitions; i++) {
        createOpenCylinder((i-1)*HEIGHT/partitions, i*HEIGHT/partitions, RADIUS);
    }

    // Create curved head, following parabolic curve
    // y = -x^2
    partitions = 15;
    float xMin = -0.25;
    float xMax = 1;
    float baseHeight = HEIGHT + xMin*xMin;
    for (int i = 0; i <= partitions; i++) {
        float x = 9*(xMax-xMin)/partitions;
        float y = -x*x;
        //createCircle(i*(baseHeight+y)/partitions);
    }


    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    int width, height, nrChannels;
    std::string texturePath = PROJECT_SOURCE_DIR "/Textures/Streetlight/metal.jpg";
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