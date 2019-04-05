#pragma once

#include "Assets/mesh.hpp"

#include <glm/glm.hpp>

#include <vector>
#include <memory>


namespace Utils
{
    class MeshCreator
    {
    public:
        MeshCreator() { }
        ~MeshCreator() { }

        std::shared_ptr<Assets::Mesh> create();

        int addOpenCylinder(float tDiff, float height1, float height2, float radius);
        void addRotatedOpenCylinder(float height1, float height2, float radius, float x1, float x2, float angle1, float angle2);
        void addSphere(float maxPhi, glm::vec3 offset, float radius, float rotation);
        void addCircularFace(float xOffset, float radius);
        void addCylinder(float height, float radius);

        std::vector<glm::vec3> mPositions;
        std::vector<glm::vec3> mNormals;
        std::vector<glm::vec2> mTexCoords;

        float mTextureScaleS = 1;
        float mTextureScaleT = 1;

    private:
        MeshCreator(MeshCreator const &) = delete;
        MeshCreator & operator=(MeshCreator const &) = delete;
    };
}