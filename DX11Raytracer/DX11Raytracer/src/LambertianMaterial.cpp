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
        //const vec3 scatterDirWS = rec.p + rec.normal + Normalize(vec3::random_in_unit_sphere()); // normalize to use Lambertian distribution
        vec3 scatterDirWS = rec.p + vec3::RandomInHemisphere(rec.normal); // normalize to use Lambertian distribution

                                                                          // Catch degenerate scatter direction
        if (scatterDirWS.IsNearlyZero())
            scatterDirWS = rec.normal;

        scattered = Ray(rec.p, scatterDirWS, rayIn.GetTime());
        attenuation = m_albedoTexture->GetColor(rec.u, rec.v, rec.p);
        return true;
    }
}