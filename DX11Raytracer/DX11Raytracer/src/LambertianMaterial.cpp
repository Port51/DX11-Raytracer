#include "LambertianMaterial.h"
#include "SolidColorTexture.h"
#include "Texture.h"

namespace gfx
{

	LambertianMaterial::LambertianMaterial(const Color& a) 
        : m_albedoTexture(std::make_shared<SolidColorTexture>(a))
    {}

    LambertianMaterial::LambertianMaterial(std::shared_ptr<Texture> texture)
        : m_albedoTexture(texture)
    {}

    bool LambertianMaterial::Scatter(const Ray& rayIn, const RayHitRecord& rec, Color& attenuation, Ray& scattered) const
    {
        //vec3 scatterDirWS = rec.normalWS + Normalize(vec3::RandomInUnitSphere()); // normalize to use Lambertian distribution
        vec3 scatterDirWS = vec3::RandomInHemisphere(rec.normalWS); // normalize to use Lambertian distribution
                                                                          // Catch degenerate scatter direction
        if (scatterDirWS.IsNearlyZero())
            scatterDirWS = rec.normalWS;

        scattered = Ray(rec.positionWS, scatterDirWS, rayIn.GetTime(), rayIn.GetRandomSeed());
        attenuation = m_albedoTexture->GetColor(rec.u, rec.v, rec.positionWS);
        return true;
    }
}