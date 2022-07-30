#pragma once
#include "Material.h"
#include "CommonHeader.h"

namespace gfx
{
    // todo: inherit from dielectric?
    class IceMaterial : public Material
    {
    public:
        IceMaterial();

    public:
        virtual const bool Scatter(const Ray& rayIn, const RayHitRecord& rec, Color& attenuation, Color& emission, Ray& scattered, const GBuffer& gBuffer, const uint gBufferIdx, const uint passIteration) const override;
        virtual const bool IsInGBuffer(const uint gBufferIdx) const override;
        static const vec3f GetIceSample(const vec3f& position, const uint octaves, const bool highQuality);

    private:
        static f32 SchlickApprox(const f32 cosine, const f32 reflectiveIdx);
        const Color GetIceRaymarch(const Ray& rayIn, const RayHitRecord& rec, const f32 previousAlpha, const uint maxRaySteps, const uint octaves, const bool highQuality, const uint passIteration) const;
    };
}