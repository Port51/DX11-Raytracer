#include "SolidColorTexture.h"

namespace gfx
{
	SolidColorTexture::SolidColorTexture(const Color c) : m_color(c) {}

    SolidColorTexture::SolidColorTexture(const double red, const double green, const double blue)
        : SolidColorTexture(Color(static_cast<float>(red), static_cast<float>(green), static_cast<float>(blue), 1.f))
    {}

    Color SolidColorTexture::GetColor(const double u, const double v, const vec3 & p) const
    {
        return m_color;
    }
}