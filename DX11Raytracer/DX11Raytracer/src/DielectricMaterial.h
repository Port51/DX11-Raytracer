#pragma once
#include "Material.h"
#include "CommonHeader.h"

namespace gfx
{
    class DielectricMaterial : public Material
    {
    public:
        DielectricMaterial(const f32 indexOfRefraction);

    public:
        virtual const bool Scatter(const Ray& rayIn, const RayHitRecord& rec, Color& attenuation, Color& emission, Ray& scattered, const GBuffer& gBuffer, const uint gBufferIdx, const uint passIteration) const override;
        virtual const bool IsInGBuffer(const uint gBufferIdx) const override;

    private:
        static f32 SchlickApprox(const f32 cosine, const f32 reflectiveIdx);

    private:
        f32 m_indexOfRefraction;
    };
}