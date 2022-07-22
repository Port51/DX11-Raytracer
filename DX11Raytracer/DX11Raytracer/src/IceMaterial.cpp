#include "IceMaterial.h"
#include "SolidColorTexture.h"
#include "Texture.h"

namespace gfx
{
	IceMaterial::IceMaterial()
	{}

	const bool IceMaterial::Scatter(const Ray & rayIn, const RayHitRecord & rec, Color & attenuation, Ray & scattered) const
	{
		// Use for debugging
		return false;

		// Snell's law:
		// n0 * sin(theta0) = n1 * sin(theta1)
		// 
		// n0 and n1 are refractive indices of materials
		// angles are relative to normals

		attenuation = Color(1.0, 1.0, 1.0, 1.0) * m_noise.GetNoise3D(rec.positionWS, 2u);

		// This assumes the other medium is air - need to update if adding more complicated situations
		const double refractionRatio = rec.isFrontFacing ? (1.0 / 1.33) : 1.33;

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

	const Color IceMaterial::GetEmission(const RayHitRecord& rec) const
	{

		// Use for debugging
		//auto debugVal = rec.positionWS.y / 0.2;
		auto debugVal = GetIceSample(rec.positionWS);
		return Color(debugVal, debugVal, debugVal, debugVal);

		// Normal output
		//return Color(0.0, 0.0, 0.0, 0.0);
	}

	double IceMaterial::SchlickApprox(const double cosine, const double reflectiveIdx)
	{
		return 0.0;
	}

	const double IceMaterial::GetIceSample(const vec3& position) const
	{
		const auto ScaleXY = 55.0;
		const auto ScaleZ  = 20.0;
		const auto n0 = m_noise.GetNoise3D(position * vec3(ScaleXY, ScaleXY, ScaleZ), 8u);
		const auto n1 = m_noise.GetNoise3D(position * vec3(ScaleXY, ScaleXY, ScaleZ) + vec3(21309.90, 3289.32, 93432.032), 8u);

		const auto cracks = std::pow(min(1.0, (1.0 - abs(n0 - n1)) * 1.115 - 0.1), 7.0);
		const auto clouds = n0 * n0 * 0.125 * (1.0 - cracks);

		return cracks + clouds;
	}
}