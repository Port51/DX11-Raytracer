#pragma once
#include "CommonHeader.h"

namespace gfx
{
    class Texture
    {
    public:
        virtual Color GetColor(const f32 u, const f32 v, const vec3f& p) const = 0;
    };
}