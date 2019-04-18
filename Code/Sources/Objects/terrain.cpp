#include "Objects/terrain.hpp"
#include "Components/physicsbody.hpp"
#include "Utils/meshcreator.hpp"

#include <stb_image.h>


using namespace Objects;

const float SIZE_X = 128.0f;
const float SIZE_Y = 10.0f;
const float SIZE_Z = 128.0f;

const float TEXTURE_REPEAT_X = 30.0f;
const float TEXTURE_REPEAT_Z = 30.0f;


namespace Objects
{
    int Terrain::mHeightmapWidth;
    int Terrain::mHeightmapHeight;
    unsigned char *Terrain::mTransposedHeightData;

    Terrain::Terrain() : Core::GameObject()
    {
        // **** CREATE COMPONENTS ****
        // Create physics body
        auto physicsBody = std::make_shared<Components::PhysicsBody>(*this);
        physicsBody->mShape = std::make_unique<btHeightfieldTerrainShape>(
            mHeightmapHeight,
            mHeightmapWidth,
            mTransposedHeightData,
            1.0/255.0*SIZE_Y,
            0,
            SIZE_Y,
            1,
            PHY_UCHAR,
            false
        );
        physicsBody->mShape->setLocalScaling(btVector3(SIZE_X/(mHeightmapWidth-1), 1, SIZE_Z/(mHeightmapHeight-1)));
        #ifdef DEBUG
            btVector3 aabbMin, aabbMax;
            physicsBody->mShape->getAabb(btTransform::getIdentity(), aabbMin, aabbMax);
            std::cout << "Min AABB: " << "(" << aabbMin[0] << ", " << aabbMin[1] << ", " << aabbMin[2] << ")" << std::endl;
            std::cout << "Max AABB: " << "(" << aabbMax[0] << ", " << aabbMax[1] << ", " << aabbMax[2] << ")" << std::endl;
        #endif
        btTransform terrainTransform;
        terrainTransform.setIdentity();
        terrainTransform.setOrigin(btVector3(0, SIZE_Y/2, 0));
        btScalar mass(0.0f);
        btVector3 localInertia(0, 0, 0);
        physicsBody->mMotionState = std::make_unique<btDefaultMotionState>(terrainTransform);
        btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, &(*physicsBody->mMotionState), &(*physicsBody->mShape), localInertia);
        physicsBody->mRigidBody = std::make_unique<btRigidBody>(rbInfo);
        addComponent(physicsBody);
    }

    Terrain::~Terrain()
    {
        delete [] mTransposedHeightData;
    }

    void Terrain::setup()
    {
        loadHeightMap(PROJECT_SOURCE_DIR "/Textures/HeightMaps/height_map1.png");
    }

    void Terrain::loadHeightMap(std::string heightMap)
    {
        int nrChannels;
        unsigned char *data = stbi_load(heightMap.c_str(), &mHeightmapWidth, &mHeightmapHeight, &nrChannels, 1);
        if (data)
        {
            // Create Bullet terrain shape
            mTransposedHeightData = (unsigned char *) malloc(mHeightmapHeight*mHeightmapWidth*1);
            for (int i = 1; i < mHeightmapHeight; i++) {
                for (int j = 1; j < mHeightmapWidth; j++) {
                    mTransposedHeightData[j*mHeightmapWidth+i] = data[i*mHeightmapWidth+j];
                }
            }

            stbi_image_free(data);
        }
        else
        {
            std::cout << "Texture failed to load at path: " << heightMap << std::endl;
            stbi_image_free(data);
        }
    }
}