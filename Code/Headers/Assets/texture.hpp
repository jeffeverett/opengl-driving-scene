#pragma once

#include <string>

namespace Assets
{
    class Texture
    {
    public:
        Texture(std::string const &path);

        std::string mPath;
        unsigned int mID;
    private:
        void loadTexture(std::string const &path);
    };
}