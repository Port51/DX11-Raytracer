#pragma once
#include "Material.h"
#include "CommonHeader.h"

namespace gfx
{
    class Texture;

    class LambertianMaterial : public Material
    {
    public:
        LambertianMaterial(const Color& a);
        LambertianMaterial(std::shared_ptr<Texture> texture);

    public:
        virtual const bool Scatter(const Ray& rayIn, const RayHitRecord& rec, Color& attenuation, Color& emission, Ray& scattered) const override;

    private:
        std::shared_ptr<Texture> m_albedoTexture;
    };
}