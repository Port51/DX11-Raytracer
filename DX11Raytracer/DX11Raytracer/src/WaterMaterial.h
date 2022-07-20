#pragma once
#include "Material.h"
#include "CommonHeader.h"

namespace gfx
{
    // todo: inherit from dielectric?
    class WaterMaterial : public Material
    {
    public:
        WaterMaterial();

    public:
        virtual const bool Scatter(const Ray& rayIn, const RayHitRecord& rec, Color& attenuation, Ray& scattered) const override;
        virtual const Color GetEmission(const RayHitRecord& rec) const override;

    private:
        static double SchlickApprox(const double cosine, const double reflectiveIdx);

    private:
    };
}