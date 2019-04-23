#pragma once

#include <string>

namespace Assets
{
    class Texture
    {
    public:
        Texture(std::string const &path, bool flipVertically = false);

        std::string mPath;
        unsigned int mID;
    private:
        void loadTexture(std::string const &path, bool flipVertically);
    };
}