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
        SolidColorTexture(const f32 red, const f32 green, const f32 blue);

    public:
        virtual Color GetColor(const f32 u, const f32 v, const vec3f& p) const override;

    private:
        const Color m_color;
    };
}