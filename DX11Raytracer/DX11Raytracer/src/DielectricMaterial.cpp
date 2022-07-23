#include "DielectricMaterial.h"
#include "GBuffer.h"

namespace gfx
{
	DielectricMaterial::DielectricMaterial(const double indexOfRefraction)
        : m_indexOfRefraction(indexOfRefraction)
    {}

    const bool DielectricMaterial::Scatter(const Ray& rayIn, const RayHitRecord& rec, Color& attenuation, Color& emission, Ray& scattered, const GBuffer& gBuffer, const uint gBufferIdx, const uint passIteration) const
    {
        // Snell's law:
        // n0 * sin(theta0) = n1 * sin(theta1)
        // 
        // n0 and n1 are refractive indices of materials
        // angles are relative to normals

        attenuation = Color(1.0, 1.0, 1.0, 1.0);
        emission = Color(0.0, 0.0, 0.0, 0.0);

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

    const bool DielectricMaterial::IsInGBuffer(const uint gBufferIdx) const
    {
        return (gBufferIdx != 1u);
    }

    double DielectricMaterial::SchlickApprox(const double cosine, const double reflectiveIdx)
    {
        // Use Schlick's approximation for reflectance.
        auto r0 = (1.0 - reflectiveIdx) / (1.0 + reflectiveIdx);
        r0 = r0 * r0;
        return r0 + (1.0 - r0) * std::pow((1.0 - cosine), 5.0);
    }
}