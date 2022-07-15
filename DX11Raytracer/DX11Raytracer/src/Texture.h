#pragma once
#include "CommonHeader.h"

namespace gfx
{
    class Texture
    {
    public:
        virtual Color GetColor(const double u, const double v, const vec3& p) const = 0;
    };
}