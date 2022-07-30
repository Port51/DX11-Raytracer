#pragma once
#include "CommonHeader.h"
#include "Texture.h"

namespace gfx
{
    class CheckeredTexture : public Texture
    {
    public:
        CheckeredTexture() = default;
        CheckeredTexture(const Color even, const Color odd);

    public:
        virtual Color GetColor(const f32 u, const f32 v, const vec3f& p) const override;

    private:
        const Color m_odd;
        const Color m_even;
    };
}