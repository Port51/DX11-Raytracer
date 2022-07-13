#pragma once
#include "Material.h"
#include "CommonHeader.h"

namespace gfx
{
    class Lambertian : public Material
    {
    public:
        Lambertian(const Color& a) : m_albedo(a) {}

        virtual bool Scatter(const Ray& rayIn, const RayHitRecord& rec, Color& attenuation, Ray& scattered) const override
        {
            //const vec3 scatterDirWS = rec.p + rec.normal + Normalize(vec3::random_in_unit_sphere()); // normalize to use Lambertian distribution
            vec3 scatterDirWS = rec.p + vec3::RandomInHemisphere(rec.normal); // normalize to use Lambertian distribution

            // Catch degenerate scatter direction
            if (scatterDirWS.IsNearlyZero())
                scatterDirWS = rec.normal;

            scattered = Ray(rec.p, scatterDirWS);
            attenuation = m_albedo;
            return true;
        }

    public:
        Color m_albedo;
    };
}