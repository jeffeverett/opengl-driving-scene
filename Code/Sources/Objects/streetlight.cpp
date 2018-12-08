#include "Objects/streetlight.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>

using namespace Objects;

glm::vec3 Streetlight::mPointlightOffset;

std::shared_ptr<Core::Drawable> Streetlight::mDrawable;
std::shared_ptr<Core::Shader> Streetlight::mShader;

std::vector<glm::vec3> Streetlight::mPostPositions;
std::vector<glm::vec3> Streetlight::mPostNormals;
std::vector<glm::vec2> Streetlight::mPostTexCoords;
std::vector<Core::Texture> Streetlight::mPostTextures;

std::vector<glm::vec3> Streetlight::mBulbPositions;
std::vector<glm::vec3> Streetlight::mBulbNormals;
std::vector<glm::vec2> Streetlight::mBulbTexCoords;
std::vector<Core::Texture> Streetlight::mBulbTextures;


const float RADIUS = 0.04f;
const float HEIGHT = 5.5f;
const float TEXTURE_SCALE_S = 0.2;
const float TEXTURE_SCALE_T = 1;
const int NR_POINT_LIGHTS = 6;
const float SEPARATION_X = 3;

Streetlight::Streetlight(glm::vec3 position, float theta, bool onLeft) : Core::GameObject(mDrawable, mShader) {
    if (onLeft) {
        scale(glm::vec3(1, 1, 1));
    }
    else {
        scale(glm::vec3(-1, 1, 1));
    }

    btCylinderShape *streetlightShape = new btCylinderShape(btVector3(RADIUS, HEIGHT, RADIUS));
    btTransform streetlightTransform;
    streetlightTransform.setIdentity();
    streetlightTransform.setOrigin(btVector3(position[0], -4, position[2]));
    btScalar mass(0.);
    btVector3 localInertia(0, 0, 0);
    btDefaultMotionState *myMotionState = new btDefaultMotionState(streetlightTransform);
    btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, streetlightShape, localInertia);
    mRigidBody = std::make_unique<btRigidBody>(rbInfo);
    dynamicsWorld->addRigidBody(&(*mRigidBody));

    setRenderRotation(theta);
}

int Streetlight::createOpenCylinder(float height1, float height2, float radius) {
    int partitions = 10;
    float theta = 360;
    for (int i = 1; i <= partitions; i++) {
        // Positions
        glm::vec3 leftTop = glm::vec3(radius*glm::cos(glm::radians((i-1)*theta/partitions)), height2, radius*glm::sin(glm::radians((i-1)*theta/partitions)));
        glm::vec3 leftBottom = glm::vec3(radius*glm::cos(glm::radians((i-1)*theta/partitions)), height1, radius*glm::sin(glm::radians((i-1)*theta/partitions)));
        glm::vec3 rightTop = glm::vec3(radius*glm::cos(glm::radians(i*theta/partitions)), height2, radius*glm::sin(glm::radians(i*theta/partitions)));
        glm::vec3 rightBottom = glm::vec3(radius*glm::cos(glm::radians(i*theta/partitions)), height1, radius*glm::sin(glm::radians(i*theta/partitions)));

        // Create first triangle
        mPostPositions.push_back(leftTop);
        mPostPositions.push_back(leftBottom);
        mPostPositions.push_back(rightBottom);
        mPostNormals.push_back(glm::vec3(leftTop[0], 0, leftTop[2]));
        mPostNormals.push_back(glm::vec3(leftBottom[0], 0, leftBottom[2]));
        mPostNormals.push_back(glm::vec3(rightBottom[0], 0, rightBottom[2]));
        mPostTexCoords.push_back(glm::vec2(TEXTURE_SCALE_S*(i-1)/partitions, TEXTURE_SCALE_S*height2));
        mPostTexCoords.push_back(glm::vec2(TEXTURE_SCALE_S*(i-1)/partitions, TEXTURE_SCALE_T*height1));
        mPostTexCoords.push_back(glm::vec2(TEXTURE_SCALE_S*i/partitions, TEXTURE_SCALE_T*height1));

        // Create second triangle
        mPostPositions.push_back(leftTop);
        mPostPositions.push_back(rightBottom);
        mPostPositions.push_back(rightTop);
        mPostNormals.push_back(glm::vec3(leftTop[0], 0, leftTop[2]));
        mPostNormals.push_back(glm::vec3(rightBottom[0], 0, rightBottom[2]));
        mPostNormals.push_back(glm::vec3(rightTop[0], 0, rightTop[2]));
        mPostTexCoords.push_back(glm::vec2(TEXTURE_SCALE_S*(i-1)/partitions, TEXTURE_SCALE_T*height2));
        mPostTexCoords.push_back(glm::vec2(TEXTURE_SCALE_S*i/partitions, TEXTURE_SCALE_T*height1));
        mPostTexCoords.push_back(glm::vec2(TEXTURE_SCALE_S*i/partitions, TEXTURE_SCALE_T*height2));
    }

    return 6*partitions;
}

void Streetlight::createRotatedOpenCylinder(float height1, float height2, float radius, float x1, float x2, float angle1, float angle2) {
    int verticesUsed = createOpenCylinder(height1, height2, radius);
    int startIdx = mPostPositions.size()-verticesUsed;

    // Rotate vertices created in previous call
    glm::mat3 rot1 = glm::eulerAngleZ(-angle1);
    glm::mat3 rot2 = glm::eulerAngleZ(-angle2);
    for (int i = startIdx; i < mPostPositions.size(); i+=6) {
        mPostPositions[i] -= glm::vec3(0, height2, 0);
        mPostPositions[i] = rot2*mPostPositions[i];
        mPostPositions[i] += glm::vec3(0, height2, 0);

        mPostPositions[i+1] -= glm::vec3(0, height1, 0);
        mPostPositions[i+1] = rot1*mPostPositions[i+1];
        mPostPositions[i+1] += glm::vec3(0, height1, 0);

        mPostPositions[i+2] -= glm::vec3(0, height1, 0);
        mPostPositions[i+2] = rot1*mPostPositions[i+2];
        mPostPositions[i+2] += glm::vec3(0, height1, 0);

        mPostPositions[i+3] -= glm::vec3(0, height2, 0);
        mPostPositions[i+3] = rot2*mPostPositions[i+3];
        mPostPositions[i+3] += glm::vec3(0, height2, 0);

        mPostPositions[i+4] -= glm::vec3(0, height1, 0);
        mPostPositions[i+4] = rot1*mPostPositions[i+4];
        mPostPositions[i+4] += glm::vec3(0, height1, 0);

        mPostPositions[i+5] -= glm::vec3(0, height2, 0);
        mPostPositions[i+5] = rot2*mPostPositions[i+5];
        mPostPositions[i+5] += glm::vec3(0, height2, 0);
    }

    // Apply x offset to vertices created in previous call
    for (int i = startIdx; i < mPostPositions.size(); i+=6) {
        mPostPositions[i] += glm::vec3(x2, 0, 0);
        mPostPositions[i+1] += glm::vec3(x1, 0, 0);
        mPostPositions[i+2] += glm::vec3(x1, 0, 0);
        mPostPositions[i+3] += glm::vec3(x2, 0, 0);
        mPostPositions[i+4] += glm::vec3(x1, 0, 0);
        mPostPositions[i+5] += glm::vec3(x2, 0, 0);
    }
}

void Streetlight::createSphere(glm::vec3 offset, float radius) {
    int thetaPartitions = 10;
    int phiPartitions = 10;

    float theta0 = 0;
    float phi0 = 0;

    // Draw lowest partition
    for (int i = 0; i <= phiPartitions; i++) {
        float phi = glm::radians(90.0*i/phiPartitions);
        for (int j = 0; j <= thetaPartitions; j++) {
            float theta = glm::radians(360.0*j/thetaPartitions);
            // Positions
            glm::vec3 leftTop = glm::vec3(radius*glm::sin(theta0)*glm::sin(phi), radius*glm::cos(phi), radius*glm::cos(theta0)*glm::sin(phi));
            glm::vec3 rightTop = glm::vec3(radius*glm::sin(theta)*glm::sin(phi), radius*glm::cos(phi), radius*glm::cos(theta)*glm::sin(phi));
            glm::vec3 leftBottom = glm::vec3(radius*glm::sin(theta0)*glm::sin(phi0), radius*glm::cos(phi0), radius*glm::cos(theta0)*glm::sin(phi0));
            glm::vec3 rightBottom = glm::vec3(radius*glm::sin(theta)*glm::sin(phi0), radius*glm::cos(phi0), radius*glm::cos(theta)*glm::sin(phi0));


            // Create first triangle
            mPostPositions.push_back(offset+leftTop);
            mPostPositions.push_back(offset+leftBottom);
            mPostPositions.push_back(offset+rightBottom);
            mPostNormals.push_back(glm::vec3(leftTop[0], 0, leftTop[2]));
            mPostNormals.push_back(glm::vec3(leftBottom[0], 0, leftBottom[2]));
            mPostNormals.push_back(glm::vec3(rightBottom[0], 0, rightBottom[2]));
            mPostTexCoords.push_back(glm::vec2(theta0/glm::radians(360.0), (90+phi)/glm::radians(180.0)));
            mPostTexCoords.push_back(glm::vec2(theta0/glm::radians(360.0), (90+phi0)/glm::radians(180.0)));
            mPostTexCoords.push_back(glm::vec2(theta/glm::radians(360.0), (90+phi0)/glm::radians(180.0)));

            // Create second triangle
            mPostPositions.push_back(offset+leftTop);
            mPostPositions.push_back(offset+rightBottom);
            mPostPositions.push_back(offset+rightTop);
            mPostNormals.push_back(glm::vec3(leftTop[0], 0, leftTop[2]));
            mPostNormals.push_back(glm::vec3(rightBottom[0], 0, rightBottom[2]));
            mPostNormals.push_back(glm::vec3(rightTop[0], 0, rightTop[2]));
            mPostTexCoords.push_back(glm::vec2(theta0/glm::radians(360.0), (90+phi)/glm::radians(180.0)));
            mPostTexCoords.push_back(glm::vec2(theta/glm::radians(360.0), (90+phi0)/glm::radians(180.0)));
            mPostTexCoords.push_back(glm::vec2(theta/glm::radians(360.0), (90+phi)/glm::radians(180.0)));

            theta0 = theta;
        }
        phi0 = phi;
    }
}

void Streetlight::updateLighting(int lightIdx) {
    // Point light
    glm::vec3 pointlightPosition = getPosition()+getWorldOffset(mPointlightOffset);
    glm::vec3 ponitlightDirection = getWorldDown();

    std::string lightNumber = std::to_string(lightIdx);
    defaultShader->use();
    defaultShader->setVec3("pointLights[" + lightNumber + "].position", pointlightPosition);
    defaultShader->setVec3("spotLights[" + lightNumber + "].direction", ponitlightDirection);

    debugDrawer->drawLine(glmVec32btVector3(pointlightPosition), glmVec32btVector3(pointlightPosition+glm::vec3(1)*ponitlightDirection), btVector3(1.0,0,0));
}

void Streetlight::setup() {
    // Setup lighting
    for (int i = 0; i < NR_POINT_LIGHTS; i++) {
        std::string number = std::to_string(i);
        defaultShader->setVec3("pointLights[" + number + "].ambient", glm::vec3(0.3,0.3,0.3));
        defaultShader->setVec3("pointLights[" + number + "].diffuse", glm::vec3(0.75,0.75,0.75));
        defaultShader->setVec3("pointLights[" + number + "].specular", glm::vec3(1,1,1));
        defaultShader->setFloat("pointLights[" + number + "].constant", 1.0f);
        defaultShader->setFloat("pointLights[" + number + "].linear", 0.09f);
        defaultShader->setFloat("pointLights[" + number + "].quadratic", 0.032f);
    }

    // Create post
    int partitions = 10;
    for (int i = 1; i <= partitions; i++) {
        createOpenCylinder((i-1)*HEIGHT/partitions, i*HEIGHT/partitions, RADIUS);
    }

    // Create curved head, following parabolic curve
    // y = -2x^2+C, where C is the constant s.t. y(xMin) = HEIGHT
    partitions = 15;
    float xMin = -0.25;
    float xMax = 0.45;
    float C = HEIGHT + 2*xMin*xMin;
    float x0 = xMin;
    float y0 = -2*xMin*xMin+C;
    for (int i = 0; i <= partitions; i++) {
        float x = xMin+i*(xMax-xMin)/partitions;
        float y = -2*x*x+C;

#ifdef DEBUG
        std::cout << "x: " << x << ", y: " << y << std::endl;
#endif

        createRotatedOpenCylinder(y0, y, RADIUS, x0-xMin, x-xMin, glm::radians(180.0f)*(i-1)/partitions, glm::radians(180.0f)*i/partitions);

        x0 = x;
        y0 = y;
    }

    // Create bulb
    createSphere(glm::vec3(x0-xMin, y0, 0), 0.2);


    // Load post texture
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

    mPostTextures.push_back(Core::Texture { textureID, "texture_diffuse", texturePath });


    // Load bulb texture
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    texturePath = PROJECT_SOURCE_DIR "/Textures/Streetlight/glass.jpg";
    data = stbi_load(texturePath.c_str(), &width, &height, &nrChannels, 0);
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

    mBulbTextures.push_back(Core::Texture { textureID, "texture_diffuse", texturePath });


    // Construct model
    std::shared_ptr<Core::Mesh> postMesh = std::make_shared<Core::Mesh>(mPostPositions, mPostNormals, mPostTexCoords, mPostTextures);
    std::shared_ptr<Core::Mesh> bulbMesh = std::make_shared<Core::Mesh>(mBulbPositions, mBulbNormals, mBulbTexCoords, mBulbTextures);

    std::shared_ptr<Core::Model> model = std::make_shared<Core::Model>();
    model->meshes.push_back(*postMesh);
    model->meshes.push_back(*bulbMesh);

    mDrawable = model;
    mShader = defaultShader;

    mPointlightOffset = glm::vec3(x0-xMin,y0,0);
}