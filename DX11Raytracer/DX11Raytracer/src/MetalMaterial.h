#pragma once
#include "Material.h"
#include "CommonHeader.h"

namespace gfx
{
    class MetalMaterial : public Material
    {
    public:
        MetalMaterial(const Color& a, const double roughness);

    public:
        virtual bool Scatter(const Ray& rayIn, const RayHitRecord& rec, Color& attenuation, Ray& scattered) const override;

    private:
        Color m_albedo;
        double m_roughness;
    };
}