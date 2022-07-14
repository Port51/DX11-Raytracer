#pragma once
#include "Material.h"
#include "CommonHeader.h"

namespace gfx
{
    class LambertianMaterial : public Material
    {
    public:
        LambertianMaterial(const Color& a);

    public:
        virtual bool Scatter(const Ray& rayIn, const RayHitRecord& rec, Color& attenuation, Ray& scattered) const override;

    private:
        Color m_albedo;
    };
}