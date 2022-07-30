#include "SolidColorTexture.h"

namespace gfx
{
	SolidColorTexture::SolidColorTexture(const Color c) : m_color(c) {}

    SolidColorTexture::SolidColorTexture(const f32 red, const f32 green, const f32 blue)
        : SolidColorTexture(Color(static_cast<f32>(red), static_cast<f32>(green), static_cast<f32>(blue), 1.f))
    {}

    Color SolidColorTexture::GetColor(const f32 u, const f32 v, const vec3f & p) const
    {
        return m_color;
    }
}