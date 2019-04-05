#include "Objects/wall.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>

using namespace Objects;

const float WALL_DEPTH = 0.15f;
const float WALL_HEIGHT = 1.3f;

const float TEXTURE_REPEAT_S = 1.0f/3.0f;
const float TEXTURE_REPEAT_T = 1.0f;


namespace Objects
{
    Wall::Wall() : Core::GameObject()
    {

    }

    void Wall::addSegment(Utils::MeshCreator &meshCreator, int segNumber, float a, float b, float theta1, float theta2)
    {
    }

    void Wall::setup()
    {
    }
}