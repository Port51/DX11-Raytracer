#pragma once
#include "CommonHeader.h"
#include "Texture.h"

namespace gfx
{
    class SolidColorTexture : public Texture
    {
    public:
        SolidColorTexture() = default;
        SolidColorTexture(const Color c);
        SolidColorTexture(const double red, const double green, const double blue);

    public:
        virtual Color GetColor(const double u, const double v, const vec3& p) const override;

    private:
        const Color m_color;
    };
}