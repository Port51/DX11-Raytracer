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
        static double SchlickApprox(const double cosine, const double reflectiveIdx);

    private:
        double m_indexOfRefraction;
    };
}