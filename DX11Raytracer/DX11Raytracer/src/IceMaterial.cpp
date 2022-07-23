#include "IceMaterial.h"
#include "SolidColorTexture.h"
#include "Texture.h"
#include "PerlinNoise.h"
#include "GBuffer.h"

namespace gfx
{
	IceMaterial::IceMaterial()
	{}

	const bool IceMaterial::Scatter(const Ray & rayIn, const RayHitRecord & rec, Color& attenuation, Color& emission, Ray& scattered, const GBuffer& gBuffer, const uint gBufferIdx, const uint passIteration) const
	{
		if (gBufferIdx == 1u)
		{
			// Raymarch pass
			emission = GetIceRaymarch(rayIn, rec, 50u, 7u, false, passIteration);
			return false;
		}

		const auto n0 = PerlinNoise::GetNoise3D(rec.positionWS * vec3(25.0, 25.0, 1.0), 7u);
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
		const double reflectBias = 1.8; // makes for better screenshot...
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
			// Sample raymarched gbuffer
			uint pixelIdx;
			if (rayIn.TryGetPixelIdx(rec.u, rec.v, pixelIdx))
			{
				emission = gBuffer.IceRaymarchCache.at(pixelIdx);
			}
			else
			{
				// This happens when a ray bounces outside the view frustum
				// In this case, do a low quality raymarch
				emission = GetIceRaymarch(rayIn, rec, 5u, 6u, false, passIteration);
			}
			return false;
		}
	}

	const bool IceMaterial::IsInGBuffer(const uint gBufferIdx) const
	{
		return true;
	}

	double IceMaterial::SchlickApprox(const double cosine, const double reflectiveIdx)
	{
		// Use Schlick's approximation for reflectance.
		auto r0 = (1.0 - reflectiveIdx) / (1.0 + reflectiveIdx);
		r0 = r0 * r0;
		return r0 + (1.0 - r0) * std::pow((1.0 - cosine), 5.0);
	}

	const Color IceMaterial::GetIceRaymarch(const Ray& rayIn, const RayHitRecord& rec, const uint maxRaySteps, const uint octaves, const bool highQuality, const uint passIteration) const
	{
		Color result = Color(0.0);

		const vec3 direction = Normalize(rayIn.GetDirection());
		auto visibility = 1.0;

		const auto maxDistance = 8.0;
		const auto stepLength = maxDistance / maxRaySteps;
		const auto offset = static_cast<double>(passIteration % RaymarchPassCt) / static_cast<double>(RaymarchPassCt) * stepLength;

		for (size_t i = 0u; i < maxRaySteps; ++i)
		{
			auto t = i * stepLength + offset;
			auto sample = GetIceSample(rec.positionWS + direction * t, octaves, highQuality);
			auto ice = sample.x;

			ice *= visibility;
			auto iceVisible = ice * visibility;

			// Exponential decay for light bounces
			result += Color(iceVisible * std::exp(t * -1.8), iceVisible * std::exp(t * -1.05), iceVisible * std::exp(t * -0.6), iceVisible);
			visibility *= (1.0 - ice);
		}

		return result;
	}

	const vec3 IceMaterial::GetIceSample(const vec3& position, const uint octaves, const bool highQuality)
	{
		const auto ScaleXZ = 11.0;
		const auto ScaleY  = 5.5;
		const auto n0 = PerlinNoise::GetNoise3D(position * vec3(ScaleXZ, ScaleY, ScaleXZ), octaves);
		const auto n1 = PerlinNoise::GetNoise3D(position * vec3(ScaleXZ, ScaleY, ScaleXZ) + vec3(21309.90, 3289.32, 93432.032), octaves);

		const auto n2 = PerlinNoise::GetNoise3D(position * vec3(ScaleXZ, ScaleY, ScaleXZ) * 2.0, octaves);
		const auto n3 = PerlinNoise::GetNoise3D(position * vec3(ScaleXZ, ScaleY, ScaleXZ) * 2.0 + vec3(21309.90, 3289.32, 93432.032), octaves);

		const auto heightSlopeQ = 10.0;
		const auto heightSlopeOffset = 0.5;

		const auto largeDifferenceNoise = Saturate((1.0 - abs(n0 - n1)) * 1.8 - 0.8);
		const auto largeHeightRatio = Saturate((n1 - n0) * heightSlopeQ + heightSlopeOffset);

		// Restrict small cracks to lower region
		const auto smallDifferenceNoise = Saturate((1.0 - abs(n2 - n3)) * 1.8 - 0.8) * (1.0 - largeHeightRatio);
		const auto smallHeightRatio = Saturate((n2 - n3) * heightSlopeQ + heightSlopeOffset);

		const auto cracks =
			std::pow(largeDifferenceNoise, 71.0) * 0.8
			+ std::pow(largeDifferenceNoise, 9.0) * 0.2
			+ std::pow(smallDifferenceNoise, 67.0) * 0.67;

		if (highQuality)
		{
			const auto n2 = PerlinNoise::GetNoise3D(position * vec3(31.0) + vec3(109.90, 289.32, 3432.032), octaves);
			const auto clouds = std::pow(n2, 6.0);
			return vec3(Saturate(cracks + clouds * 0.00035), largeHeightRatio, smallHeightRatio);
		}
		else
		{
			return vec3(Saturate(cracks), largeHeightRatio, smallHeightRatio);
		}
	}
}