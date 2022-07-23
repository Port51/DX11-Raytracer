#include "IceMaterial.h"
#include "SolidColorTexture.h"
#include "Texture.h"
#include "PerlinNoise.h"

namespace gfx
{
	IceMaterial::IceMaterial()
	{}

	const bool IceMaterial::Scatter(const Ray & rayIn, const RayHitRecord & rec, Color& attenuation, Color& emission, Ray& scattered) const
	{
		// Use for debugging
		//return false;

		// Snell's law:
		// n0 * sin(theta0) = n1 * sin(theta1)
		// 
		// n0 and n1 are refractive indices of materials
		// angles are relative to normals

		const auto n0 = PerlinNoise::GetNoise3D(rec.positionWS * vec3(25.0, 25.0, 1.0), 8u);
		const auto roughness = std::pow(SCurve(n0), 10.0);

		attenuation = Color(1.0, 1.0, 1.0, 1.0) * PerlinNoise::GetNoise3D(rec.positionWS, 2u);

		// This assumes the other medium is air - need to update if adding more complicated situations
		const double refractionRatio = rec.isFrontFacing ? (1.0 / 1.33) : 1.33; // should be 1.33, but this looks better...

		vec3 lambertScatterDirWS = vec3::RandomInHemisphere(rec.normalWS);
		if (lambertScatterDirWS.IsNearlyZero())
			lambertScatterDirWS = rec.normalWS;

		const vec3 rayDirNorm = Normalize(rayIn.GetDirection());

		const double cosTheta = fmin(Dot(rayDirNorm, rec.normalWS), 1.0);
		const double sinTheta = sqrt(1.0 - cosTheta * cosTheta);

		//const bool totalInternalReflection = refractionRatio * sinTheta > 1.0;
		const double reflectBias = 2.0; // makes for better screenshot...
		const bool fresnelReflection = (SchlickApprox(cosTheta, refractionRatio) * reflectBias) > rayIn.GetRandomSeed();

		//auto sch = SchlickApprox(cosTheta, refractionRatio);
		//emission = Color(sch, 0.0, 0.0, 0.0);
		//return false;

		// Either reflect or refract
		if (fresnelReflection)
		//if (totalInternalReflection || fresnelReflection)
		{
			const vec3 direction = Reflect(rayDirNorm, rec.normalWS) + roughness * vec3::RandomInUnitSphere();
			scattered = Ray(rec.positionWS, direction, rayIn.GetTime(), rayIn.GetRandomSeed());
			emission = Color(0.0, 0.0, 0.0, 0.0);
			return true;
		}
		else
		{
			// Do raymarch
			//const vec3 direction = Normalize(Refract(rayDirNorm, rec.normalWS, refractionRatio) + roughness * vec3::RandomInUnitSphere());
			const vec3 direction = Normalize(rayDirNorm + roughness * vec3::RandomInUnitSphere());
			emission = Color(0.0, 0.0, 0.0, 0.0);
			auto visibility = 1.0;

			const uint maxRaySteps = 1000u;
			const auto maxDistance = 7.0;
			const auto stepLength = maxDistance / maxRaySteps;

			for (size_t i = 0u; i < maxRaySteps; ++i)
			{
				auto t = i * stepLength;
				auto ice = GetIceSample(rec.positionWS + direction * t);

				ice *= visibility;
				auto iceVisible = ice * visibility;

				// Exponential decay for light bounces
				emission += Color(iceVisible * std::exp(t * -2.0), iceVisible * std::exp(t * -1.2), iceVisible * std::exp(t * -0.7), iceVisible);
				visibility *= (1.0 - ice);
			}

			return false;
		}
	}

	const Color IceMaterial::GetEmission(const RayHitRecord& rec) const
	{

		// Use for debugging
		//auto debugVal = rec.positionWS.y / 0.2;
		auto debugVal = GetIceSample(rec.positionWS);
		//if (rec.positionWS.y > 0.01) debugVal = 0;
		//return Color(debugVal, debugVal, debugVal, debugVal);

		// Normal output
		return Color(0.0, 0.0, 0.0, 0.0);
	}

	double IceMaterial::SchlickApprox(const double cosine, const double reflectiveIdx)
	{
		// Use Schlick's approximation for reflectance.
		auto r0 = (1.0 - reflectiveIdx) / (1.0 + reflectiveIdx);
		r0 = r0 * r0;
		return r0 + (1.0 - r0) * std::pow((1.0 - cosine), 5.0);
	}

	const double IceMaterial::GetIceSample(const vec3& position) const
	{
		const auto ScaleXZ = 11.0;
		const auto ScaleY  = 0.5;
		const auto n0 = PerlinNoise::GetNoise3D(position * vec3(ScaleXZ, ScaleY, ScaleXZ), 9u);
		const auto n1 = PerlinNoise::GetNoise3D(position * vec3(ScaleXZ, ScaleY, ScaleXZ) + vec3(21309.90, 3289.32, 93432.032), 9u);
		const auto n2 = PerlinNoise::GetNoise3D(position * vec3(51.0) + vec3(209.90, 289.32, 3432.032), 9u);

		const auto cracks = std::pow(Saturate((1.0 - abs(n0 - n1)) * 1.8 - 0.8), 71.0);
		const auto clouds = std::pow(n2, 7.0);

		return cracks + clouds * 0.000031;
	}
}