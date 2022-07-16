#include "DielectricMaterial.h"

namespace gfx
{
	DielectricMaterial::DielectricMaterial(const double indexOfRefraction)
        : m_indexOfRefraction(indexOfRefraction)
    {}

    bool DielectricMaterial::Scatter(const Ray& rayIn, const RayHitRecord& rec, Color& attenuation, Ray& scattered) const
    {
        // Snell's law:
        // n0 * sin(theta0) = n1 * sin(theta1)
        // 
        // n0 and n1 are refractive indices of materials
        // angles are relative to normals

        attenuation = Color(1.0, 1.0, 1.0, 1.0);

        // This assumes the other medium is air - need to update if adding more complicated situations
        const double refractionRatio = rec.isFrontFacing ? (1.0 / m_indexOfRefraction) : m_indexOfRefraction;

        const vec3 rayDirNorm = Normalize(rayIn.GetDirection());

        const double cosTheta = fmin(Dot(-rayDirNorm, rec.normalWS), 1.0);
        const double sinTheta = sqrt(1.0 - cosTheta * cosTheta);

        const bool totalInternalReflection = refractionRatio * sinTheta > 1.0;
        const bool fresnelReflection = SchlickApprox(cosTheta, refractionRatio) > rayIn.GetRandomSeed();

        // Either reflect or refract
        vec3 direction;
        if (totalInternalReflection || fresnelReflection)
            direction = Reflect(rayDirNorm, rec.normalWS);
        else
            direction = Refract(rayDirNorm, rec.normalWS, refractionRatio);

        scattered = Ray(rec.positionWS, direction, rayIn.GetTime(), rayIn.GetRandomSeed());
        return true;
    }
}