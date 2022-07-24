#pragma once
#include "CommonHeader.h"
#include <vector>

namespace gfx
{
    class GBuffer
    {
    public:
        GBuffer(const int screenWidth, const int screenHeight);
        ~GBuffer() = default;
    public:
        std::vector<Color> CameraColor;
        std::vector<Color> IceRaymarchCache;
    };
}