#include "Objects/terrain.hpp"

using namespace Objects;

const float SIZE_X = 128.0f;
const float SIZE_Y = 10.0f;
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
    mMotionState = std::make_unique<btDefaultMotionState>(groundTransform);
    btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, &(*mMotionState), &(*mTerrainShape), localInertia);
    mRigidBody = std::make_unique<btRigidBody>(rbInfo);
    dynamicsWorld->addRigidBody(&(*mRigidBody));

}

Terrain::~Terrain() {
    delete [] mTransposedHeightData;
}

void Terrain::setup() {
    // Generate mesh
    Core::MeshCreator meshCreator;
    loadHeightMap(meshCreator, mHeightMaps[0]);
    meshCreator.addTexture(PROJECT_SOURCE_DIR "/Textures/Ground/rock.jpg");
    mDrawable = meshCreator.create();

    mShader = defaultShader;
}

void Terrain::loadHeightMap(Core::MeshCreator &meshCreator, std::string heightMap)
{
    int width, height, nrChannels;
    unsigned char *data = stbi_load(heightMap.c_str(), &width, &height, &nrChannels, 1);
    if (data)
    {
        for (int i = 1; i < height; i++) {
            for (int j = 1; j < width; j++) {
                // Positions
                glm::vec3 topLeft(i/(width-1.0)*SIZE_X, data[i*width + (j-1)]/255.0*SIZE_Y, (j-1)/(height-1.0)*SIZE_Z);
                glm::vec3 topRight(i/(width-1.0)*SIZE_X, data[i*width + j]/255.0*SIZE_Y, j/(height-1.0)*SIZE_Z);
                glm::vec3 bottomLeft((i-1)/(width-1.0)*SIZE_X, data[(i-1)*width + (j-1)]/255.0*SIZE_Y, (j-1)/(height-1.0)*SIZE_Z);
                glm::vec3 bottomRight((i-1)/(width-1.0)*SIZE_X, data[(i-1)*width + j]/255.0*SIZE_Y, j/(height-1.0)*SIZE_Z);
                meshCreator.mPositions.insert(meshCreator.mPositions.end(), {
                    topLeft, bottomLeft, bottomRight,
                    topLeft, bottomRight, topRight
                });
            }
        }

        // Calculate normals
        for (int i = 0; i < meshCreator.mPositions.size(); i+=3) {
            glm::vec3 vec1 = meshCreator.mPositions[i+1]-meshCreator.mPositions[i];
            glm::vec3 vec2 = meshCreator.mPositions[i+2]-meshCreator.mPositions[i+1];
            glm::vec3 normal = glm::cross(vec1, vec2);

            meshCreator.mNormals.insert(meshCreator.mNormals.end(), {
                normal, normal, normal
            });
        }
        // Determine texture coordinates
        for (auto &position : meshCreator.mPositions) {
            meshCreator.mTexCoords.push_back(glm::vec2(position[0]/SIZE_X*TEXTURE_REPEAT_X, position[2]/SIZE_Z*TEXTURE_REPEAT_Z));
        }

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

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << heightMap << std::endl;
        stbi_image_free(data);
    }
}