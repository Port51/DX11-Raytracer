#include "DielectricMaterial.h"
#include "GBuffer.h"

namespace gfx
{
	DielectricMaterial::DielectricMaterial(const f32 indexOfRefraction)
        : m_indexOfRefraction(indexOfRefraction)
    {}

    const bool DielectricMaterial::Scatter(const Ray& rayIn, const RayHitRecord& rec, Color& attenuation, Color& emission, Ray& scattered, const GBuffer& gBuffer, const uint gBufferIdx, const uint passIteration) const
    {
        // Snell's law:
        // n0 * sin(theta0) = n1 * sin(theta1)
        // 
        // n0 and n1 are refractive indices of materials
        // angles are relative to normals

        attenuation = Color(1.0f, 1.0f, 1.0f, 1.0f);
        emission = Color(0.0f, 0.0f, 0.0f, 0.0f);

        // This assumes the other medium is air - need to update if adding more complicated situations
        const f32 refractionRatio = rec.isFrontFacing ? (1.0f / m_indexOfRefraction) : m_indexOfRefraction;

        const f32 cosTheta = fmin(Dot(-rayIn.GetDirection(), rec.normalWS), 1.0f);
        const f32 sinTheta = sqrt(1.0f - cosTheta * cosTheta);

        const bool totalInternalReflection = refractionRatio * sinTheta > 1.0f;
        const bool fresnelReflection = SchlickApprox(cosTheta, refractionRatio) > rayIn.GetRandomSeed();

        // Either reflect or refract
        vec3f direction;
        if (totalInternalReflection || fresnelReflection)
            direction = Reflect(rayIn.GetDirection(), rec.normalWS);
        else
            direction = Refract(rayIn.GetDirection(), rec.normalWS, refractionRatio);

        scattered = Ray(rayIn, rec.positionWS, direction);
        return true;
    }

    const bool DielectricMaterial::IsInGBuffer(const uint gBufferIdx) const
    {
        return (gBufferIdx != 1u);
    }

    f32 DielectricMaterial::SchlickApprox(const f32 cosine, const f32 reflectiveIdx)
    {
        // Use Schlick's approximation for reflectance.
        f32 r0 = (1.0f - reflectiveIdx) / (1.0f + reflectiveIdx);
        r0 = r0 * r0;
        return r0 + (1.0f - r0) * std::pow((1.0f - cosine), 5.0f);
    }
}