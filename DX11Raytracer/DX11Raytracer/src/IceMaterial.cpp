#include "IceMaterial.h"
#include "SolidColorTexture.h"
#include "Texture.h"
#include "PerlinNoise.h"
#include "GBuffer.h"
#include "Settings.h"

namespace gfx
{
	IceMaterial::IceMaterial()
	{}

	const bool IceMaterial::Scatter(const Ray & rayIn, const RayHitRecord & rec, Color& attenuation, Color& emission, Ray& scattered, const GBuffer& gBuffer, const uint gBufferIdx, const uint passIteration) const
	{
		if (gBufferIdx == 1u)
		{
			// Raymarch pass
			emission = GetIceRaymarch(rayIn, rec, 50u, 7u, true, passIteration);
			return false;
		}

		const auto n0 = PerlinNoise::GetNoise3D(rec.positionWS * vec3(55.0, 1.0, 55.0), 7u);
		const auto roughness = std::pow(Saturate(n0), 3.0) * 0.15;

		attenuation = Color(1.0f) * PerlinNoise::GetNoise3D(rec.positionWS, 2u);

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
				emission = GetIceRaymarch(rayIn, rec, 5u, 6u, true, passIteration);
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
			result += Color(iceVisible * std::exp(t * -1.51), iceVisible * std::exp(t * -0.881), iceVisible * std::exp(t * -0.5121), iceVisible);
			visibility *= (1.0 - ice);
		}

		return result;
	}

	const vec3 IceMaterial::GetIceSample(const vec3& position, const uint octaves, const bool highQuality)
	{
		const auto ScaleXZ = 11.0;
		const auto ScaleY  = 4.1;
		auto n0 = PerlinNoise::GetNoise3D(position * vec3(ScaleXZ, ScaleY, ScaleXZ), octaves);
		auto n1 = PerlinNoise::GetNoise3D(position * vec3(ScaleXZ, ScaleY, ScaleXZ) + vec3(21309.90, 3289.32, 93432.032), octaves);

		auto n2 = PerlinNoise::GetNoise3D(position * vec3(ScaleXZ, ScaleY, ScaleXZ) * 4.0, octaves - 3u);
		auto n3 = PerlinNoise::GetNoise3D(position * vec3(ScaleXZ, ScaleY, ScaleXZ) * 4.0 + vec3(21309.90, 3289.32, 93432.032), octaves - 3u);

		const double heightSlopeQ = 10.0;
		const double heightSlopeOffset = 0.5;

		double largeHeightRatio = Saturate((n1 - n0) * heightSlopeQ + heightSlopeOffset);
		//const auto smallHeightRatio = Saturate((n2 - n3) * heightSlopeQ + heightSlopeOffset) * (1.0 - largeHeightRatio);
		double smallHeightRatio = 0.0;

		// Create a cave!
		if (highQuality)
		{
			const vec3 caveStart = vec3(2.0, -2.25, 0.0);
			const vec3 caveRight = vec3(0.0, 0.0, 1.0);

			vec3 caveOffset = vec3(position.z - caveStart.x, (position.y - caveStart.y) * 0.65, 0.0);
			const double caveSdf = caveOffset.Length();

			double isCave = Saturate(1.0 - caveSdf * 0.551);// * (largeHeightRatio > 0.0);
			//isCave = 1.0;
			n0 = Lerp(n0, 1.0, isCave);
			n1 = Lerp(n1, 0.0, isCave);
			//n2 = Lerp(n2, 1.0, isCave);
			//n3 = Lerp(n3, 0.0, isCave);
			largeHeightRatio *= (1.0 - isCave);
		}
		const double largeDifferenceNoise = Saturate((1.0 - abs(n0 - n1)) * 1.8 - 0.8);
		// Restrict small cracks to ice region
		const auto smallDifferenceNoise = Saturate((1.0 - abs(n2 - n3)) * 1.8 - 0.8) * largeHeightRatio;

		const double spread = Saturate(position.y / 0.42);
		const double cracks =
			std::pow(largeDifferenceNoise, 71.0 - 70.0 * spread)
			+ std::pow(smallDifferenceNoise, 31.0) * 0.272;

		if (highQuality)
		{
			const double n2 = PerlinNoise::GetNoise3D(position * vec3(131.0) + vec3(109.90, 289.32, 3432.032), octaves);
			const double clouds = (std::pow(n2, 2.7) + n2 * 0.333) * largeHeightRatio;
			return vec3(Saturate(cracks + clouds * 0.003251), largeHeightRatio, smallHeightRatio);
		}
		else
		{
			return vec3(Saturate(cracks), largeHeightRatio, smallHeightRatio);
		}
	}
}