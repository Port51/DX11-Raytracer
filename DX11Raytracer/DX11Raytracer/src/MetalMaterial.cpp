#include "MetalMaterial.h"
#include "SolidColorTexture.h"
#include "GBuffer.h"

namespace gfx
{

	MetalMaterial::MetalMaterial(const Color& a, const double roughness)
        : m_albedoTexture(std::move(std::make_shared<SolidColorTexture>(a))), m_roughness(roughness)
    {}

    MetalMaterial::MetalMaterial(std::shared_ptr<Texture> texture, const double roughness)
        : m_albedoTexture(std::move(texture)), m_roughness(roughness)
    {}

    const bool MetalMaterial::Scatter(const Ray& rayIn, const RayHitRecord& rec, Color& attenuation, Color& emission, Ray& scattered, const GBuffer& gBuffer, const uint gBufferIdx, const uint passIteration) const
    {
        vec3 reflectedDir = Reflect(rayIn.GetDirection(), rec.normalWS);
        scattered = Ray(rayIn, rec.positionWS, reflectedDir + m_roughness * vec3::RandomInUnitSphere());
        attenuation = m_albedoTexture->GetColor(rec.u, rec.v, rec.positionWS);
        emission = Color(0.0, 0.0, 0.0, 0.0);
        return (Dot(scattered.GetDirection(), rec.normalWS) > 0);
    }
    const bool MetalMaterial::IsInGBuffer(const uint gBufferIdx) const
    {
        return (gBufferIdx != 1u);
    }
}