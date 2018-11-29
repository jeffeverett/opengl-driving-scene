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

std::vector<glm::vec3> housePositions {
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

std::vector<glm::vec3> houseNormals {
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
    glm::vec3(0.0f, -1.0f, 0.0f),
};

std::vector<glm::vec2> houseTexCoords {
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



// Define Namespace
namespace Objects
{
    class House : public Utils::GameObject
    {
    public:

        // Implement Default Constructor and Destructor
        House() : Utils::GameObject(mDrawable, mShader) {
            // Call setup before constructor

            btBoxShape *houseShape = new btBoxShape(btVector3(btScalar(1.), btScalar(1.), btScalar(1.)));
            btTransform houseTransform;
            houseTransform.setIdentity();
            houseTransform.setOrigin(btVector3(0, 0.0, -10));
            btScalar mass(0.);
            btVector3 localInertia(0, 0, 0);
            btDefaultMotionState *myMotionState = new btDefaultMotionState(houseTransform);
            btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, houseShape, localInertia);
            mRigidBody = std::make_unique<btRigidBody>(rbInfo);
            dynamicsWorld->addRigidBody(&(*mRigidBody));

        }
        ~House() { }


        // Public Member Functions
        static void setup() {
            GLuint textureID;
            glGenTextures(1, &textureID);
            glBindTexture(GL_TEXTURE_2D, textureID);

            int width, height, nrChannels;
            std::string texturePath = PROJECT_SOURCE_DIR "/Textures/House/brick.jpg";
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
            mDrawable = std::make_shared<Utils::Mesh>(housePositions, houseNormals, houseTexCoords, mTextures);
            mShader = defaultShader;
        }

    private:

        // Disable Copying and Assignment
        House(House const &) = delete;
        House & operator=(House const &) = delete;

        // Private members
        static std::shared_ptr<Utils::Drawable> mDrawable;
        static std::shared_ptr<Utils::Shader> mShader;
        static std::vector<Utils::Texture> mTextures;
    };

    std::shared_ptr<Utils::Drawable> House::mDrawable;
    std::shared_ptr<Utils::Shader> House::mShader;
    std::vector<Utils::Texture> House::mTextures;
}
