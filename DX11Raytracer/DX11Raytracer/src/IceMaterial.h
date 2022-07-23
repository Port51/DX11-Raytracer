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
        virtual const bool Scatter(const Ray& rayIn, const RayHitRecord& rec, Color& attenuation, Color& emission, Ray& scattered) const override;
        virtual const Color GetEmission(const RayHitRecord& rec) const override;

    private:
        static double SchlickApprox(const double cosine, const double reflectiveIdx);
        const double GetIceSample(const vec3& position) const;
    };
}