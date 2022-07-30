#pragma once
#include "CommonHeader.h"

namespace gfx
{
    class Skybox
    {
    public:
        Skybox(const Color& upwardColor, const Color& horizonColor, const Color& downwardColor);
        ~Skybox() = default;

    public:
        const Color GetColor(const vec3& direction);

    private:
        Color m_upwardColor;
        Color m_horizonColor;
        Color m_downwardColor;
    };
}