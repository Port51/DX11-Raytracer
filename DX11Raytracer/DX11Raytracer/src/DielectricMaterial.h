#pragma once
#include "Material.h"
#include "CommonHeader.h"

namespace gfx
{
    class DielectricMaterial : public Material
    {
    public:
        DielectricMaterial(const double indexOfRefraction);

    public:
        virtual bool Scatter(const Ray& rayIn, const RayHitRecord& rec, Color& attenuation, Ray& scattered) const override;

    private:
        static double SchlickApprox(const double cosine, const double reflectiveIdx)
        {
            // Use Schlick's approximation for reflectance.
            auto r0 = (1.0 - reflectiveIdx) / (1.0 + reflectiveIdx);
            r0 = r0 * r0;
            return r0 + (1.0 - r0) * std::pow((1.0 - cosine), 5.0);
        }

    private:
        double m_indexOfRefraction;
    };
}