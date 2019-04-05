#pragma once

#include "Core/gameobject.hpp"
#include "Assets/mesh.hpp"
#include "Assets/material.hpp"

#include <vector>
#include <memory>


namespace Objects
{
    class Terrain : public Core::GameObject
    {
    public:
        Terrain();
        ~Terrain();


        static void setup();

    private:
        Terrain(Terrain const &) = delete;
        Terrain & operator=(Terrain const &) = delete;

        static void loadHeightMap(std::string heightMap);

        static std::shared_ptr<Assets::Mesh> mMesh;
        static std::shared_ptr<Assets::Material> mMaterial;
    };
}
