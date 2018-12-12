#include "Objects/streetlight.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>

using namespace Objects;

glm::vec3 Streetlight::mPointlightOffset;

std::shared_ptr<Core::Drawable> Streetlight::mDrawable;
std::shared_ptr<Core::Shader> Streetlight::mShader;

const float RADIUS = 0.04f;
const float HEIGHT = 2.5f;
const float ROTATION = glm::radians(150.0f);
const float TEXTURE_SCALE_S = 1;
const float TEXTURE_SCALE_T = 4;
const int NR_POINT_LIGHTS = 6;

Streetlight::Streetlight(glm::vec3 position, float theta, bool onLeft) : Core::GameObject(mDrawable, mShader) {
    if (onLeft) {
        scale(glm::vec3(1, 1, 1));
    }
    else {
        scale(glm::vec3(-1, 1, 1));
    }

    mShape = std::make_unique<btCylinderShape>(btVector3(RADIUS, HEIGHT, RADIUS));
    btTransform streetlightTransform;
    streetlightTransform.setIdentity();
    streetlightTransform.setOrigin(btVector3(position[0], 0, position[2]));
    btScalar mass(0.0f);
    btVector3 localInertia(0, 0, 0);
    mMotionState = std::make_unique<btDefaultMotionState>(streetlightTransform);
    btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, &(*mMotionState), &(*mShape), localInertia);
    mRigidBody = std::make_unique<btRigidBody>(rbInfo);
    dynamicsWorld->addRigidBody(&(*mRigidBody));

    setRenderRotation(theta);
}

int Streetlight::createOpenCylinder(Core::MeshCreator &meshCreator, float tDiff, float height1, float height2, float radius) {
    int partitions = 10;
    float theta = 360;
    for (int i = 1; i <= partitions; i++) {
        // Positions
        glm::vec3 leftTop = glm::vec3(radius*glm::cos(glm::radians((i-1)*theta/partitions)), height2, radius*glm::sin(glm::radians((i-1)*theta/partitions)));
        glm::vec3 leftBottom = glm::vec3(radius*glm::cos(glm::radians((i-1)*theta/partitions)), height1, radius*glm::sin(glm::radians((i-1)*theta/partitions)));
        glm::vec3 rightTop = glm::vec3(radius*glm::cos(glm::radians(i*theta/partitions)), height2, radius*glm::sin(glm::radians(i*theta/partitions)));
        glm::vec3 rightBottom = glm::vec3(radius*glm::cos(glm::radians(i*theta/partitions)), height1, radius*glm::sin(glm::radians(i*theta/partitions)));

        // Create first triangle
        meshCreator.mPositions.push_back(leftTop);
        meshCreator.mPositions.push_back(leftBottom);
        meshCreator.mPositions.push_back(rightBottom);
        meshCreator.mNormals.push_back(glm::vec3(leftTop[0], 0, leftTop[2]));
        meshCreator.mNormals.push_back(glm::vec3(leftBottom[0], 0, leftBottom[2]));
        meshCreator.mNormals.push_back(glm::vec3(rightBottom[0], 0, rightBottom[2]));
        meshCreator.mTexCoords.push_back(glm::vec2(TEXTURE_SCALE_S*(i-1)/partitions, TEXTURE_SCALE_T*(height1+tDiff)));
        meshCreator.mTexCoords.push_back(glm::vec2(TEXTURE_SCALE_S*(i-1)/partitions, TEXTURE_SCALE_T*height1));
        meshCreator.mTexCoords.push_back(glm::vec2(TEXTURE_SCALE_S*i/partitions, TEXTURE_SCALE_T*height1));

        // Create second triangle
        meshCreator.mPositions.push_back(leftTop);
        meshCreator.mPositions.push_back(rightBottom);
        meshCreator.mPositions.push_back(rightTop);
        meshCreator.mNormals.push_back(glm::vec3(leftTop[0], 0, leftTop[2]));
        meshCreator.mNormals.push_back(glm::vec3(rightBottom[0], 0, rightBottom[2]));
        meshCreator.mNormals.push_back(glm::vec3(rightTop[0], 0, rightTop[2]));
        meshCreator.mTexCoords.push_back(glm::vec2(TEXTURE_SCALE_S*(i-1)/partitions, TEXTURE_SCALE_T*(height1+tDiff)));
        meshCreator.mTexCoords.push_back(glm::vec2(TEXTURE_SCALE_S*i/partitions, TEXTURE_SCALE_T*height1));
        meshCreator.mTexCoords.push_back(glm::vec2(TEXTURE_SCALE_S*i/partitions, TEXTURE_SCALE_T*(height1+tDiff)));
    }

    return 6*partitions;
}

void Streetlight::createRotatedOpenCylinder(Core::MeshCreator &meshCreator, float height1, float height2, float radius, float x1, float x2, float angle1, float angle2) {
    int verticesUsed = createOpenCylinder(meshCreator, x2-x1, height1, height2, radius);
    unsigned long startIdx = meshCreator.mPositions.size()-verticesUsed;

    // Rotate vertices created in previous call
    glm::mat3 rot1 = glm::eulerAngleZ(-angle1);
    glm::mat3 rot2 = glm::eulerAngleZ(-angle2);
    for (unsigned long i = startIdx; i < meshCreator.mPositions.size(); i+=6) {
        meshCreator.mPositions[i] -= glm::vec3(0, height2, 0);
        meshCreator.mPositions[i] = rot2*meshCreator.mPositions[i];
        meshCreator.mPositions[i] += glm::vec3(0, height2, 0);

        meshCreator.mPositions[i+1] -= glm::vec3(0, height1, 0);
        meshCreator.mPositions[i+1] = rot1*meshCreator.mPositions[i+1];
        meshCreator.mPositions[i+1] += glm::vec3(0, height1, 0);

        meshCreator.mPositions[i+2] -= glm::vec3(0, height1, 0);
        meshCreator.mPositions[i+2] = rot1*meshCreator.mPositions[i+2];
        meshCreator.mPositions[i+2] += glm::vec3(0, height1, 0);

        meshCreator.mPositions[i+3] -= glm::vec3(0, height2, 0);
        meshCreator.mPositions[i+3] = rot2*meshCreator.mPositions[i+3];
        meshCreator.mPositions[i+3] += glm::vec3(0, height2, 0);

        meshCreator.mPositions[i+4] -= glm::vec3(0, height1, 0);
        meshCreator.mPositions[i+4] = rot1*meshCreator.mPositions[i+4];
        meshCreator.mPositions[i+4] += glm::vec3(0, height1, 0);

        meshCreator.mPositions[i+5] -= glm::vec3(0, height2, 0);
        meshCreator.mPositions[i+5] = rot2*meshCreator.mPositions[i+5];
        meshCreator.mPositions[i+5] += glm::vec3(0, height2, 0);
    }

    // Apply x offset to vertices created in previous call
    for (unsigned long i = startIdx; i < meshCreator.mPositions.size(); i+=6) {
        meshCreator.mPositions[i] += glm::vec3(x2, 0, 0);
        meshCreator.mPositions[i+1] += glm::vec3(x1, 0, 0);
        meshCreator.mPositions[i+2] += glm::vec3(x1, 0, 0);
        meshCreator.mPositions[i+3] += glm::vec3(x2, 0, 0);
        meshCreator.mPositions[i+4] += glm::vec3(x1, 0, 0);
        meshCreator.mPositions[i+5] += glm::vec3(x2, 0, 0);
    }
}

void Streetlight::createSphere(Core::MeshCreator &meshCreator, float maxPhi, glm::vec3 offset, float radius, float rotation) {
    int thetaPartitions = 10;
    int phiPartitions = 10;

    float theta0 = 0;
    float phi0 = 0;

    // Draw lowest partition
    glm::mat3 rot = glm::eulerAngleZ(rotation);
    for (int i = 0; i <= phiPartitions; i++) {
        float phi = glm::radians(maxPhi*i/phiPartitions);
        for (int j = 0; j <= thetaPartitions; j++) {
            float theta = glm::radians(360.0f*j/thetaPartitions);
            // Positions
            glm::vec3 leftTop = glm::vec3(radius*glm::sin(theta0)*glm::sin(phi0), radius*glm::cos(phi0), radius*glm::cos(theta0)*glm::sin(phi0));
            glm::vec3 rightTop = glm::vec3(radius*glm::sin(theta)*glm::sin(phi0), radius*glm::cos(phi0), radius*glm::cos(theta)*glm::sin(phi0));
            glm::vec3 leftBottom = glm::vec3(radius*glm::sin(theta0)*glm::sin(phi), radius*glm::cos(phi), radius*glm::cos(theta0)*glm::sin(phi));
            glm::vec3 rightBottom = glm::vec3(radius*glm::sin(theta)*glm::sin(phi), radius*glm::cos(phi), radius*glm::cos(theta)*glm::sin(phi));


            // Create first triangle
            meshCreator.mPositions.push_back(offset+rot*leftTop);
            meshCreator.mPositions.push_back(offset+rot*leftBottom);
            meshCreator.mPositions.push_back(offset+rot*rightBottom);
            meshCreator.mNormals.push_back(leftTop);
            meshCreator.mNormals.push_back(leftBottom);
            meshCreator.mNormals.push_back(rightBottom);
            meshCreator.mTexCoords.push_back(glm::vec2(theta0/glm::radians(360.0f), phi0/glm::radians(maxPhi)));
            meshCreator.mTexCoords.push_back(glm::vec2(theta0/glm::radians(360.0f), phi/glm::radians(maxPhi)));
            meshCreator.mTexCoords.push_back(glm::vec2(theta/glm::radians(360.0f), phi/glm::radians(maxPhi)));

            // Create second triangle
            meshCreator.mPositions.push_back(offset+rot*leftTop);
            meshCreator.mPositions.push_back(offset+rot*rightBottom);
            meshCreator.mPositions.push_back(offset+rot*rightTop);
            meshCreator.mNormals.push_back(leftTop);
            meshCreator.mNormals.push_back(rightBottom);
            meshCreator.mNormals.push_back(rightTop);
            meshCreator.mTexCoords.push_back(glm::vec2(theta0/glm::radians(360.0), phi0/glm::radians(maxPhi)));
            meshCreator.mTexCoords.push_back(glm::vec2(theta/glm::radians(360.0), phi/glm::radians(maxPhi)));
            meshCreator.mTexCoords.push_back(glm::vec2(theta/glm::radians(360.0), phi0/glm::radians(maxPhi)));

            theta0 = theta;
        }
        phi0 = phi;
    }
}

void Streetlight::updateLighting(int lightIdx) {
    // Point light
    glm::vec3 pointlightPosition = getPosition()+getWorldOffset(mPointlightOffset);
    glm::mat3 rot = glm::eulerAngleZ(glm::radians(180.0f)-ROTATION);
    glm::vec3 pointlightDirection = getWorldOffset(rot*glm::vec3(0,-1,0));

    std::string lightNumber = std::to_string(lightIdx);
    defaultShader->use();
    defaultShader->setVec3("pointLights[" + lightNumber + "].position", pointlightPosition);
    defaultShader->setVec3("pointLights[" + lightNumber + "].direction", pointlightDirection);

    debugDrawer->drawLine(glmVec32btVector3(pointlightPosition), glmVec32btVector3(pointlightPosition+glm::vec3(1)*pointlightDirection), btVector3(1.0,0,0));
}

void Streetlight::setup() {

    // Setup lighting
    for (int i = 0; i < NR_POINT_LIGHTS; i++) {
        std::string number = std::to_string(i);
        defaultShader->setVec3("pointLights[" + number + "].ambient", glm::vec3(0.25,0.25,0.25));
        defaultShader->setVec3("pointLights[" + number + "].diffuse", glm::vec3(0.60,0.60,0.60));
        defaultShader->setVec3("pointLights[" + number + "].specular", glm::vec3(1,1,1));
        defaultShader->setFloat("pointLights[" + number + "].constant", 1.0f);
        defaultShader->setFloat("pointLights[" + number + "].linear", 0.09f);
        defaultShader->setFloat("pointLights[" + number + "].quadratic", 0.032f);
    }

    // Create post mesh
    Core::MeshCreator postMeshCreator;


    // Create post
    int partitions = 10;
    for (int i = 1; i <= partitions; i++) {
        createOpenCylinder(postMeshCreator, HEIGHT/partitions, (i-1)*HEIGHT/partitions, i*HEIGHT/partitions, RADIUS);
    }

    // Create curved head, following parabolic curve
    // y = -Ax^2+C, where C is the constant s.t. y(xMin) = HEIGHT
    partitions = 15;
    float A = 0.25f;
    float xMin = -0.25f;
    float xMax = 0.85f;
    float C = HEIGHT + A*xMin*xMin;
    float x0 = xMin;
    float y0 = -A*xMin*xMin+C;
    for (int i = 0; i <= partitions; i++) {
        float x = xMin+i*(xMax-xMin)/partitions;
        float y = -A*x*x+C;

#ifdef DEBUG
        std::cout << "x: " << x << ", y: " << y << std::endl;
#endif

        createRotatedOpenCylinder(postMeshCreator, y0, y, RADIUS, x0-xMin, x-xMin, ROTATION*(i-1)/partitions, ROTATION*i/partitions);

        x0 = x;
        y0 = y;
    }

    // Create bulb guard shape
    createSphere(postMeshCreator, 90.0f, glm::vec3(x0-xMin, y0, 0), 0.2, glm::radians(180.0f)-ROTATION);

    postMeshCreator.addTexture(PROJECT_SOURCE_DIR "/Textures/Streetlight/metal.jpg");


    // Create bulb
    Core::MeshCreator bulbMeshCreator;
    createSphere(bulbMeshCreator, 180.0f, glm::vec3(x0-xMin, y0, 0), 0.18, 0);


    bulbMeshCreator.addTexture(PROJECT_SOURCE_DIR "/Textures/Streetlight/glass.jpg");


    // Construct model
    auto postMesh = postMeshCreator.create();
    auto bulbMesh = bulbMeshCreator.create();
    std::shared_ptr<Core::Model> model = std::make_shared<Core::Model>();
    model->meshes.push_back(*postMesh);
    model->meshes.push_back(*bulbMesh);

    mDrawable = model;
    mShader = defaultShader;

    mPointlightOffset = glm::vec3(x0-xMin,y0,0);
}