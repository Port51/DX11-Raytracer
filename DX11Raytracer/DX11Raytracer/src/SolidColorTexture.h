#pragma once
#include "CommonHeader.h"
#include "Texture.h"

namespace gfx
{
    class SolidColorTexture : public Texture
    {
    public:
        SolidColorTexture() {}
        SolidColorTexture(Color c) : m_color(c) {}

        SolidColorTexture(double red, double green, double blue)
            : SolidColorTexture(Color(red, green, blue, 1.0))
        {}

        virtual Color GetColor(const double u, const double v, const vec3& p) const override
        {
            return m_color;
        }

    private:
        Color m_color;
    };
}