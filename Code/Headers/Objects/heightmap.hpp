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

const float SIZE = 50.0f;
const float TEXTURE_REPEAT = 30.0f;

std::vector<glm::vec3> groundPositions {
    glm::vec3(-SIZE,  0.0f, -SIZE),
    glm::vec3(SIZE,  0.0f, -SIZE),
    glm::vec3(SIZE,  0.0f,  SIZE),
    glm::vec3(SIZE,  0.0f,  SIZE),
    glm::vec3(-SIZE,  0.0f,  SIZE),
    glm::vec3(-SIZE,  0.0f, -SIZE)
};

std::vector<glm::vec3> groundNormals {
    glm::vec3(0.0f, 1.0f, 0.0f),
    glm::vec3(0.0f, 1.0f, 0.0f),
    glm::vec3(0.0f, 1.0f, 0.0f),
    glm::vec3(0.0f, 1.0f, 0.0f),
    glm::vec3(0.0f, 1.0f, 0.0f),
    glm::vec3(0.0f, 1.0f, 0.0f),
};

std::vector<glm::vec2> groundTexCoords {
    glm::vec2(0, TEXTURE_REPEAT),
    glm::vec2(0, 0),
    glm::vec2(TEXTURE_REPEAT, 0),
    glm::vec2(TEXTURE_REPEAT, 0),
    glm::vec2(TEXTURE_REPEAT, TEXTURE_REPEAT),
    glm::vec2(0, TEXTURE_REPEAT)
};



// Define Namespace
namespace Objects
{
    class Ground : public Utils::GameObject
    {
    public:

        // Implement Default Constructor and Destructor
        Ground() : Utils::GameObject(mDrawable, mShader) {
            // Call setup before constructor

            setOffset(glm::vec3(0, 50, 0));

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
        ~Ground() { }


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
            mDrawable = std::make_shared<Utils::Mesh>(groundPositions, groundNormals, groundTexCoords, mTextures);
            mShader = defaultShader;
        }

    private:

        // Disable Copying and Assignment
        Ground(Ground const &) = delete;
        Ground & operator=(Ground const &) = delete;

        // Private members
        static std::shared_ptr<Utils::Drawable> mDrawable;
        static std::shared_ptr<Utils::Shader> mShader;
        static std::vector<Utils::Texture> mTextures;
    };

    std::shared_ptr<Utils::Drawable> Ground::mDrawable;
    std::shared_ptr<Utils::Shader> Ground::mShader;
    std::vector<Utils::Texture> Ground::mTextures;
}
