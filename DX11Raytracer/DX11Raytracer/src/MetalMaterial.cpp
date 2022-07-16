#include "MetalMaterial.h"
#include "SolidColorTexture.h"

namespace gfx
{

	MetalMaterial::MetalMaterial(const Color& a, const double roughness)
        : m_albedoTexture(std::make_shared<SolidColorTexture>(a)), m_roughness(roughness)
    {}

    MetalMaterial::MetalMaterial(std::shared_ptr<Texture> texture, const double roughness)
        : m_albedoTexture(texture), m_roughness(roughness)
    {}

    bool MetalMaterial::Scatter(const Ray& rayIn, const RayHitRecord& rec, Color& attenuation, Ray& scattered) const
    {
        vec3 reflected = Reflect(Normalize(rayIn.GetDirection()), rec.normalWS);
        scattered = Ray(rec.positionWS, reflected + m_roughness * vec3::RandomInUnitSphere(), rayIn.GetTime());
        attenuation = m_albedoTexture->GetColor(rec.u, rec.v, rec.positionWS);
        return (Dot(scattered.GetDirection(), rec.normalWS) > 0);
    }
}