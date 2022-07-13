#pragma once
#include "Material.h"
#include "CommonHeader.h"

namespace gfx
{
    class Metal : public Material
    {
    public:
        Metal(const Color& a, const double roughness) : m_albedo(a), m_roughness(roughness) {}

        virtual bool Scatter(const Ray& rayIn, const RayHitRecord& rec, Color& attenuation, Ray& scattered) const override
        {
            vec3 reflected = Reflect(Normalize(rayIn.GetDirection()), rec.normal);
            scattered = Ray(rec.p, reflected + m_roughness * vec3::RandomInUnitSphere());
            attenuation = m_albedo;
            return (Dot(scattered.GetDirection(), rec.normal) > 0);
        }

    public:
        Color m_albedo;
        double m_roughness;
    };
}