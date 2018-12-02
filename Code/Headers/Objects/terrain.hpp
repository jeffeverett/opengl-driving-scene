#pragma once

// Local Headers
#include "globals.hpp"
#include "Utils/gameobject.hpp"
#include "Utils/mesh.hpp"

// System Headers
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <stb_image.h>

// Standard Headers
#include <vector>
#include <memory>

const float SIZE_X = 100.0f;
const float SIZE_Y = 3.0f;
const float SIZE_Z = 100.0f;

const float TEXTURE_REPEAT_X = 30.0f;
const float TEXTURE_REPEAT_Z = 30.0f;


// Define Namespace
namespace Objects
{
    class Terrain : public Utils::GameObject
    {
    public:

        // Implement Default Constructor and Destructor
        Terrain() : Utils::GameObject(mDrawable, mShader) {
            // Call setup before constructor

            setOffset(glm::vec3(-50, 50, -50));

            btBoxShape *groundShape = new btBoxShape(btVector3(btScalar(50.), btScalar(50.), btScalar(50.)));
            btTransform groundTransform;
            groundTransform.setIdentity();
            groundTransform.setOrigin(btVector3(0, -50, 0));
            btScalar mass(0.);
            btVector3 localInertia(0, 0, 0);
            btDefaultMotionState *myMotionState = new btDefaultMotionState(groundTransform);
            btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, groundShape, localInertia);
            mRigidBody = std::make_unique<btRigidBody>(rbInfo);
            dynamicsWorld->addRigidBody(&(*mRigidBody));

        }
        ~Terrain() { }


        // Public Member Functions
        static void setup() {
            GLuint textureID;
            glGenTextures(1, &textureID);
            glBindTexture(GL_TEXTURE_2D, textureID);

            int width, height, nrChannels;
            std::string texturePath = PROJECT_SOURCE_DIR "/Textures/Ground/asphalt.jpg";
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
            mTextures.push_back(Utils::Texture { textureID, "texture_diffuse", texturePath });

            loadHeightMap(mHeightMaps[0]);
            mShader = defaultShader;
        }

    private:

        static void loadHeightMap(std::string heightMap)
        {
            int width, height, nrChannels;
            unsigned char *data = stbi_load(heightMap.c_str(), &width, &height, &nrChannels, 0);
            if (data)
            {
                std::vector<glm::vec3> terrainPositions;
                std::vector<glm::vec3> terrainNormals;
                std::vector<glm::vec2> terrainTexCoords;

                for (int i = 1; i < height; i++) {
                    for (int j = 1; j < width; j++) {
                        terrainPositions.insert(terrainPositions.end(), {
                            // Bottom left
                            glm::vec3((i-1)/(width-1.0)*SIZE_X, data[(i-1)*width*nrChannels + (j-1)*nrChannels]/255.0*SIZE_Y, (j-1)/(height-1.0)*SIZE_Z),
                            // Top left
                            glm::vec3(i/(width-1.0)*SIZE_X, data[i*width*nrChannels + (j-1)*nrChannels]/255.0*SIZE_Y, (j-1)/(height-1.0)*SIZE_Z),
                            // Top right
                            glm::vec3(i/(width-1.0)*SIZE_X, data[i*width*nrChannels + j*nrChannels]/255.0*SIZE_Y, j/(height-1.0)*SIZE_Z),

                            // Bottom left
                            glm::vec3((i-1)/(width-1.0)*SIZE_X, data[(i-1)*width*nrChannels + (j-1)*nrChannels]/255.0*SIZE_Y, (j-1)/(height-1.0)*SIZE_Z),
                            // Top right
                            glm::vec3(i/(width-1.0)*SIZE_X, data[i*width*nrChannels + j*nrChannels]/255.0*SIZE_Y, j/(height-1.0)*SIZE_Z),
                            // Bottom right
                            glm::vec3((i-1)/(width-1.0)*SIZE_X, data[(i-1)*width*nrChannels + j*nrChannels]/255.0*SIZE_Y, j/(height-1.0)*SIZE_Z),

                        });
                    }
                }
                for (auto &position : terrainPositions) {
                    terrainNormals.push_back(glm::vec3(0, 1, 0));
                    terrainTexCoords.push_back(glm::vec2(position[0]/SIZE_X*TEXTURE_REPEAT_X, position[2]/SIZE_Z*TEXTURE_REPEAT_Z));
                }
                mDrawable = std::make_shared<Utils::Mesh>(terrainPositions, terrainNormals, terrainTexCoords, mTextures);
                stbi_image_free(data);
            }
            else
            {
                std::cout << "Texture failed to load at path: " << heightMap << std::endl;
                stbi_image_free(data);
            }
        }

        // Disable Copying and Assignment
        Terrain(Terrain const &) = delete;
        Terrain & operator=(Terrain const &) = delete;

        // Private members
        static std::shared_ptr<Utils::Drawable> mDrawable;
        static std::shared_ptr<Utils::Shader> mShader;
        static std::vector<Utils::Texture> mTextures;
        static std::vector<std::string> mHeightMaps;
    };

    std::shared_ptr<Utils::Drawable> Terrain::mDrawable;
    std::shared_ptr<Utils::Shader> Terrain::mShader;
    std::vector<Utils::Texture> Terrain::mTextures;
    std::vector<std::string> Terrain::mHeightMaps = {
        PROJECT_SOURCE_DIR "/Textures/HeightMaps/height_map0.jpg"
    };
}
