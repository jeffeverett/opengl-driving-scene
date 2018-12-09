#pragma once

// Local Headers
#include "globals.hpp"
#include "Core/gameobject.hpp"
#include "Core/mesh.hpp"

// System Headers
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <stb_image.h>
#include <BulletCollision/CollisionShapes/btHeightfieldTerrainShape.h>

// Standard Headers
#include <vector>
#include <memory>


// Define Namespace
namespace Objects
{
    class Terrain : public Core::GameObject
    {
    public:

        // Implement Default Constructor and Destructor
        Terrain();
        ~Terrain();


        // Public Member Functions
        static void setup();

    private:

        static void loadHeightMap(std::string heightMap);

        // Disable Copying and Assignment
        Terrain(Terrain const &) = delete;
        Terrain & operator=(Terrain const &) = delete;

        // Private members
        static std::shared_ptr<Core::Drawable> mDrawable;
        static std::shared_ptr<Core::Shader> mShader;
        static std::vector<Core::Texture> mTextures;
        static std::vector<std::string> mHeightMaps;
        static std::shared_ptr<btHeightfieldTerrainShape> mTerrainShape;
        static unsigned char* mTransposedHeightData;
    };
}
