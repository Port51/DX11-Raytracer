#pragma once
#include "Material.h"
#include "CommonHeader.h"

namespace gfx
{
    class Texture;

    class MetalMaterial : public Material
    {
    public:
        MetalMaterial(const Color& a, const double roughness);
        MetalMaterial(std::shared_ptr<Texture> texture, const double roughness);

    public:
        virtual bool Scatter(const Ray& rayIn, const RayHitRecord& rec, Color& attenuation, Ray& scattered) const override;

    private:
        std::shared_ptr<Texture> m_albedoTexture;
        double m_roughness;
    };
}