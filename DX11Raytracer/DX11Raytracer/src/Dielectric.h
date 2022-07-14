#pragma once
#include "Material.h"
#include "CommonHeader.h"

namespace gfx
{
    class Dielectric : public Material
    {
    public:
        Dielectric(double index_of_refraction) : ir(index_of_refraction) {}

        virtual bool Scatter(const Ray& rayIn, const RayHitRecord& rec, Color& attenuation, Ray& scattered) const override
        {
            attenuation = Color(1.0, 1.0, 1.0, 1.0);
            double refraction_ratio = rec.isFrontFacing ? (1.0 / ir) : ir;

            vec3 unit_direction = Normalize(rayIn.GetDirection());

            double cos_theta = fmin(Dot(-unit_direction, rec.normal), 1.0);
            double sin_theta = sqrt(1.0 - cos_theta * cos_theta);

            bool cannot_refract = refraction_ratio * sin_theta > 1.0;
            vec3 direction;

            if (cannot_refract || reflectance(cos_theta, refraction_ratio) > random_double())
                direction = Reflect(unit_direction, rec.normal);
            else
                direction = Refract(unit_direction, rec.normal, refraction_ratio);

            scattered = Ray(rec.p, direction);
            return true;
        }

    private:
        static double reflectance(double cosine, double ref_idx)
        {
            // Use Schlick's approximation for reflectance.
            auto r0 = (1 - ref_idx) / (1 + ref_idx);
            r0 = r0 * r0;
            return r0 + (1 - r0) * pow((1 - cosine), 5);
        }

    public:
        double ir; // Index of Refraction
    };
}