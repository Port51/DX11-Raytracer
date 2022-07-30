#include "SolidColorTexture.h"
#include "CheckeredTexture.h"

namespace gfx
{
    CheckeredTexture::CheckeredTexture(const Color even, const Color odd)
        : m_even(even), m_odd(odd)
    {}

    Color CheckeredTexture::GetColor(const f32 u, const f32 v, const vec3f & p) const
    {
        const f32 scale = 2.0;
        const vec3f frac = vec3f(Frac(p.x * scale), Frac(p.y * scale), Frac(p.z * scale)) - 0.5;

        const f32 checker = frac.x * frac.y * frac.z;
        return (checker < 0.0) ? m_odd : m_even;
    }
}