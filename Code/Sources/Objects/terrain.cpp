#include "Objects/terrain.hpp"

using namespace Objects;

const float SIZE_X = 128.0f;
const float SIZE_Y = 25.0f;
const float SIZE_Z = 128.0f;

const float TEXTURE_REPEAT_X = 30.0f;
const float TEXTURE_REPEAT_Z = 30.0f;

std::shared_ptr<Core::Drawable> Terrain::mDrawable;
std::shared_ptr<Core::Shader> Terrain::mShader;
std::vector<Core::Texture> Terrain::mTextures;
std::vector<std::string> Terrain::mHeightMaps = {
        PROJECT_SOURCE_DIR "/Textures/HeightMaps/height_map1.png"
};
std::shared_ptr<btHeightfieldTerrainShape> Terrain::mTerrainShape;
unsigned char* Terrain::mTransposedHeightData;


Terrain::Terrain() : Core::GameObject(mDrawable, mShader) {
    // Call setup before constructor

    setOffset(glm::vec3(-SIZE_X/2, -SIZE_Y/2, -SIZE_Z/2));

    btTransform groundTransform;
    groundTransform.setIdentity();
    groundTransform.setOrigin(btVector3(0, SIZE_Y/2, 0));
    btScalar mass(0.);
    btVector3 localInertia(0, 0, 0);
    btDefaultMotionState *myMotionState = new btDefaultMotionState(groundTransform);
    btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, &(*mTerrainShape), localInertia);
    mRigidBody = std::make_unique<btRigidBody>(rbInfo);
    dynamicsWorld->addRigidBody(&(*mRigidBody));

}

Terrain::~Terrain() {
    delete [] mTransposedHeightData;
}

void Terrain::setup() {
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    int width, height, nrChannels;
    std::string texturePath = PROJECT_SOURCE_DIR "/Textures/Ground/rock.jpg";
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
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    mTextures.push_back(Core::Texture { textureID, "texture_diffuse", texturePath });

    loadHeightMap(mHeightMaps[0]);
    mShader = defaultShader;
}

void Terrain::loadHeightMap(std::string heightMap)
{
    int width, height, nrChannels;
    unsigned char *data = stbi_load(heightMap.c_str(), &width, &height, &nrChannels, 1);
    if (data)
    {
        // Generate mesh
        std::vector<glm::vec3> terrainPositions;
        std::vector<glm::vec3> terrainNormals;
        std::vector<glm::vec2> terrainTexCoords;

        for (int i = 1; i < height; i++) {
            for (int j = 1; j < width; j++) {
                terrainPositions.insert(terrainPositions.end(), {
                    // Bottom left
                    glm::vec3((i-1)/(width-1.0)*SIZE_X, data[(i-1)*width + (j-1)]/255.0*SIZE_Y, (j-1)/(height-1.0)*SIZE_Z),
                    // Top left
                    glm::vec3(i/(width-1.0)*SIZE_X, data[i*width + (j-1)]/255.0*SIZE_Y, (j-1)/(height-1.0)*SIZE_Z),
                    // Top right
                    glm::vec3(i/(width-1.0)*SIZE_X, data[i*width + j]/255.0*SIZE_Y, j/(height-1.0)*SIZE_Z),

                    // Bottom left
                    glm::vec3((i-1)/(width-1.0)*SIZE_X, data[(i-1)*width + (j-1)]/255.0*SIZE_Y, (j-1)/(height-1.0)*SIZE_Z),
                    // Top right
                    glm::vec3(i/(width-1.0)*SIZE_X, data[i*width + j]/255.0*SIZE_Y, j/(height-1.0)*SIZE_Z),
                    // Bottom right
                    glm::vec3((i-1)/(width-1.0)*SIZE_X, data[(i-1)*width + j]/255.0*SIZE_Y, j/(height-1.0)*SIZE_Z),

                });
            }
        }
        for (auto &position : terrainPositions) {
            terrainNormals.push_back(glm::vec3(0, 1, 0));
            terrainTexCoords.push_back(glm::vec2(position[0]/SIZE_X*TEXTURE_REPEAT_X, position[2]/SIZE_Z*TEXTURE_REPEAT_Z));
        }
        mDrawable = std::make_shared<Core::Mesh>(terrainPositions, terrainNormals, terrainTexCoords, mTextures);

        // Create Bullet terrain shape
        mTransposedHeightData = (unsigned char *) malloc(height*width*1);
        for (int i = 1; i < height; i++) {
            for (int j = 1; j < width; j++) {
                mTransposedHeightData[j*width+i] = data[i*width+j];
            }
        }
        mTerrainShape = std::make_shared<btHeightfieldTerrainShape>(
            height,
            width,
            mTransposedHeightData,
            1.0/255.0*SIZE_Y,
            0,
            SIZE_Y,
            1,
            PHY_UCHAR,
            false
        );
        mTerrainShape->setLocalScaling(btVector3(SIZE_X/(width-1), 1, SIZE_Z/(height-1)));
#ifdef DEBUG
        btVector3 aabbMin, aabbMax;
        mTerrainShape->getAabb(btTransform::getIdentity(), aabbMin, aabbMax);
        std::cout << "Min AABB: " << "(" << aabbMin[0] << ", " << aabbMin[1] << ", " << aabbMin[2] << ")" << std::endl;
        std::cout << "Max AABB: " << "(" << aabbMax[0] << ", " << aabbMax[1] << ", " << aabbMax[2] << ")" << std::endl;
#endif

        //stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << heightMap << std::endl;
        stbi_image_free(data);
    }
}