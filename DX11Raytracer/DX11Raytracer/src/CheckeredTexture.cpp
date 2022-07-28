#include "SolidColorTexture.h"
#include "CheckeredTexture.h"

namespace gfx
{
    CheckeredTexture::CheckeredTexture(const Color even, const Color odd)
        : m_even(even), m_odd(odd)
    {}

    Color CheckeredTexture::GetColor(const double u, const double v, const vec3 & p) const
    {
        const double scale = 2.0;
        const vec3 frac = vec3(Frac(p.x * scale), Frac(p.y * scale), Frac(p.z * scale)) - 0.5;

        const double checker = frac.x * frac.y * frac.z;
        return (checker < 0.0) ? m_odd : m_even;
    }
}