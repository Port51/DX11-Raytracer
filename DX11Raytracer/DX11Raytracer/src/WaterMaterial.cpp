#include "WaterMaterial.h"
#include "SolidColorTexture.h"
#include "Texture.h"

namespace gfx
{
	WaterMaterial::WaterMaterial()
	{}

	const bool WaterMaterial::Scatter(const Ray & rayIn, const RayHitRecord & rec, Color & attenuation, Ray & scattered) const
	{
		// Use for debugging
		return false;

		attenuation = Color(1.0, 1.0, 0, 1.0);
		scattered = Ray(rec.positionWS, rec.normalWS, rayIn.GetTime(), rayIn.GetRandomSeed());
		return false;
	}

	const Color WaterMaterial::GetEmission(const RayHitRecord& rec) const
	{
		// Use for debugging
		auto debugVal = rec.positionWS.y / 0.2;
		return Color(debugVal, debugVal, debugVal, debugVal);

		// Normal output
		return Color(0.0, 0.0, 0.0, 0.0);
	}

	double WaterMaterial::SchlickApprox(const double cosine, const double reflectiveIdx)
	{
		return 0.0;
	}
}