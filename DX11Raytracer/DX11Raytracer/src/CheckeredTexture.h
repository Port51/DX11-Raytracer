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
        virtual Color GetColor(const double u, const double v, const vec3& p) const override;

    private:
        const Color m_odd;
        const Color m_even;
    };
}