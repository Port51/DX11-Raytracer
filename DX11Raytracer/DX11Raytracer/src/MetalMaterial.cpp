#include "MetalMaterial.h"

namespace gfx
{

	MetalMaterial::MetalMaterial(const Color& a, const double roughness)
        : m_albedo(a), m_roughness(roughness)
    {}

    bool MetalMaterial::Scatter(const Ray& rayIn, const RayHitRecord& rec, Color& attenuation, Ray& scattered) const
    {
        vec3 reflected = Reflect(Normalize(rayIn.GetDirection()), rec.normal);
        scattered = Ray(rec.p, reflected + m_roughness * vec3::RandomInUnitSphere(), rayIn.GetTime());
        attenuation = m_albedo;
        return (Dot(scattered.GetDirection(), rec.normal) > 0);
    }
}