#include "Objects/terrain.hpp"
#include "Utils/meshcreator.hpp"

using namespace Objects;

const float SIZE_X = 128.0f;
const float SIZE_Y = 10.0f;
const float SIZE_Z = 128.0f;

const float TEXTURE_REPEAT_X = 30.0f;
const float TEXTURE_REPEAT_Z = 30.0f;


namespace Objects
{
    Terrain::Terrain() : Core::GameObject()
    {
    }

    Terrain::~Terrain()
    {
    }

    void Terrain::setup()
    {
    }

    void Terrain::loadHeightMap(std::string heightMap)
    {
    }
}