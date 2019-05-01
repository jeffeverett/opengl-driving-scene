#pragma once

#include "Components/component.hpp"
#include "Assets/mesh.hpp"

namespace Components
{
    class MeshFilter : public Component
    {
    public:
        MeshFilter(Core::GameObject &gameObject);
        virtual ~MeshFilter();

        std::shared_ptr<Assets::Mesh> mMesh;

    private:
        MeshFilter(MeshFilter const &) = delete;
        MeshFilter &operator=(MeshFilter const &) = delete;
    };
}
