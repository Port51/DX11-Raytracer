#include "LambertianMaterial.h"
#include "SolidColorTexture.h"
#include "Texture.h"
#include "GBuffer.h"

namespace gfx
{

	LambertianMaterial::LambertianMaterial(const Color& a) 
        : m_albedoTexture(std::move(std::make_shared<SolidColorTexture>(a)))
    {}

    LambertianMaterial::LambertianMaterial(std::shared_ptr<Texture> texture)
        : m_albedoTexture(std::move(texture))
    {}

    const bool LambertianMaterial::Scatter(const Ray& rayIn, const RayHitRecord& rec, Color& attenuation, Color& emission, Ray& scattered, const GBuffer& gBuffer, const uint bufferIdx) const
    {
        //vec3 scatterDirWS = rec.normalWS + Normalize(vec3::RandomInUnitSphere()); // normalize to use Lambertian distribution
        vec3 scatterDirWS = vec3::RandomInHemisphere(rec.normalWS); // normalize to use Lambertian distribution
                                                                          // Catch degenerate scatter direction
        if (scatterDirWS.IsNearlyZero())
            scatterDirWS = rec.normalWS;

        scattered = Ray(rec.positionWS, scatterDirWS, rayIn.GetTime(), rayIn.GetRandomSeed());
        attenuation = m_albedoTexture->GetColor(rec.u, rec.v, rec.positionWS);
        emission = Color(0.0, 0.0, 0.0, 0.0);
        return true;
    }
}