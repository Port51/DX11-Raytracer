#pragma once
#include "CommonHeader.h"
#include "Texture.h"

namespace gfx
{
    class CheckeredTexture : public Texture
    {
    public:
        CheckeredTexture() {}

        CheckeredTexture(Color even, Color odd)
            : m_even(even), m_odd(odd)
        {}

        virtual Color GetColor(const double u, const double v, const vec3& p) const override
        {
            const auto scale = 2.0;
            const vec3 frac = vec3(Frac(p.x * scale), Frac(p.y * scale), Frac(p.z * scale)) - 0.5;

            const float checker = frac.x * frac.y * frac.z;
            return (checker < 0.0) ? m_odd : m_even;
        }

    public:
        Color m_odd;
        Color m_even;
    };
}