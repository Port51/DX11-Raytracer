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
			// Execute raymarch pass, getting visibility from alpha of previous results
			uint pixelIdx;
			Color prevColor = Color(0.f, 0.f, 0.f, 1.f);
			if (rayIn.TryGetPixelIdx(rec.u, rec.v, pixelIdx))
			{
				prevColor = gBuffer.IceRaymarchCache.at(pixelIdx);
			}
			emission = GetIceRaymarch(rayIn, rec, prevColor.a, RaymarchStepsPerPass, 7u, true, passIteration);
			return false;
		}

		const f32 n0 = PerlinNoise::GetNoise3D(rec.positionWS * vec3f(55.0f, 1.0f, 55.0f), 7u);
		const f32 roughness = std::pow(Saturate(n0), 3.0f) * 0.15f;

		// This assumes the other medium is air - need to update if adding more complicated situations
		const f32 refractionRatio = rec.isFrontFacing ? (1.0f / 1.33f) : 1.33f; // should be 1.33, but this looks better...

		vec3f lambertScatterDirWS = vec3f::RandomInHemisphere(rec.normalWS);
		if (lambertScatterDirWS.IsNearlyZero())
			lambertScatterDirWS = rec.normalWS;

		const f32 cosTheta = fmin(Dot(rayIn.GetDirection(), rec.normalWS), 1.0f);
		const f32 sinTheta = sqrt(1.0f - cosTheta * cosTheta);

		const bool totalInternalReflection = refractionRatio * sinTheta > 1.0f;
		const f32 reflectivity = Lerp(1.f, 0.85f, rec.materialSubIndex); // lower reflectivity of top of ice
		const f32 reflectionRatio = SchlickApprox(cosTheta, refractionRatio) * reflectivity;

		// DEBUG VIEWs:
		//emission = Color(totalInternalReflection ? 1.0 : 0.0);
		//emission = Color(reflectionRatio);
		//emission = Color(rec.materialSubIndex, rec.materialSubIndex, 1.f, 1.f);
		//emission = Color(abs(rec.normalWS.y), (f32)rec.materialSubIndex, 0.0, 1.0);
		//return false;

		// Refraction
		{
			// Sample raymarched gbuffer
			uint pixelIdx;
			if (rayIn.TryGetPixelIdx(rec.u, rec.v, pixelIdx))
			{
				emission = gBuffer.IceRaymarchCache.at(pixelIdx) * (1.0f - reflectionRatio);
			}
			else
			{
				// This happens when a ray bounces outside the view frustum
				// In this case, do a low quality raymarch
				emission = GetIceRaymarch(rayIn, rec, 1.0f, 5u, 6u, true, passIteration) * (1.0f - reflectionRatio);
			}
		}

		// Reflection
		{
			const vec3f direction = Reflect(rayIn.GetDirection(), rec.normalWS) + roughness * vec3f::RandomInUnitSphere();
			scattered = Ray(rayIn, rec.positionWS, direction);
			attenuation = Color(reflectionRatio);
		}

		return true;
	}

	const bool IceMaterial::IsInGBuffer(const uint gBufferIdx) const
	{
		return true;
	}

	f32 IceMaterial::SchlickApprox(const f32 cosine, const f32 reflectiveIdx)
	{
		// Use Schlick's approximation for reflectance.
		f32 r0 = (1.0f - reflectiveIdx) / (1.0f + reflectiveIdx);
		r0 = r0 * r0;
		return r0 + (1.0f - r0) * std::pow((1.0f - cosine), 5.0f);
	}

	const Color IceMaterial::GetIceRaymarch(const Ray& rayIn, const RayHitRecord& rec, const f32 previousAlpha, const uint maxRaySteps, const uint octaves, const bool highQuality, const uint passIteration) const
	{
		const f32 visibilityStopThreshold = 0.001f;

		Color result = Color(0.f);
		result.a = previousAlpha;

		if (result.a < visibilityStopThreshold) return result;

		const vec3f direction = Refract(rayIn.GetDirection(), -rec.normalWS, 1.f / 1.33f);

		f32 sectionLength;
		f32 stepLength;
		f32 offset;

		if (!UseRaymarchSlices)
		{
			// Iterate across small section
			sectionLength = MaxRaymarchDistance / RaymarchPassCt;
			stepLength = sectionLength / maxRaySteps;
			offset = static_cast<f32>(passIteration % RaymarchPassCt) * sectionLength;
		}
		else
		{
			// Original "comb" method implemented
			// This causes inaccuracy with occlusion (but keep the code for demonstration purposes)
			stepLength = MaxRaymarchDistance / maxRaySteps;
			offset = static_cast<f32>(passIteration % RaymarchPassCt) / static_cast<f32>(RaymarchPassCt) * stepLength;
		}

		for (size_t i = 0u; i < maxRaySteps; ++i)
		{
			const f32 t = i * stepLength + offset;
			const vec3f sample = GetIceSample(rec.positionWS + direction * t, octaves, highQuality);
			f32 ice = sample.x * RaymarchDensity;
			f32 iceVisible = ice * result.a;

			// Exponential decay for light bounces
			const f32 depthScale = 0.91f;
			result += Color(iceVisible * std::exp(t * -1.51f * depthScale), iceVisible * std::exp(t * -0.881f * depthScale), iceVisible * std::exp(t * -0.5121f * depthScale), 0.0f);
			result.a *= (1.f - ice);

			if (result.a < visibilityStopThreshold) return result;
		}

		return result;
	}

	const vec3f IceMaterial::GetIceSample(const vec3f& position, const uint octaves, const bool highQuality)
	{
		const f32 ScaleXZ = 11.0f;
		const f32 ScaleY  = 4.1f;
		f32 n0 = PerlinNoise::GetNoise3D(position * vec3f(ScaleXZ, ScaleY, ScaleXZ), octaves);
		f32 n1 = PerlinNoise::GetNoise3D(position * vec3f(ScaleXZ, ScaleY, ScaleXZ) + vec3f(21309.90f, 3289.32f, 93432.032f), octaves);

		f32 n2 = PerlinNoise::GetNoise3D(position * vec3f(ScaleXZ, ScaleY, ScaleXZ) * 4.0f, octaves - 1u);
		f32 n3 = PerlinNoise::GetNoise3D(position * vec3f(ScaleXZ, ScaleY, ScaleXZ) * 4.0f + vec3f(21309.90f, 3289.32f, 93432.032f), octaves - 1u);

		const f32 heightSlopeQ = 51.0f;
		const f32 heightSlopeOffset = 0.1f;

		// Create a cave!
		f32 isCave = 0.0f;
		if (highQuality)
		{
			const vec3f caveStart = vec3f(2.0f, -3.35f, 0.0f);
			const vec3f caveRight = vec3f(0.0f, 0.0f, 1.0f);

			vec3f caveOffset = vec3f(position.z - caveStart.x, (position.y - caveStart.y) * 0.65f, 0.0f);
			const f32 caveSdf = caveOffset.Length();

			isCave = Saturate(1.0f - caveSdf * 0.551f);
			n0 = Lerp(n0, 1.0f, isCave);
			n1 = Lerp(n1, 0.0f, isCave);
		}
		const f32 isIceSurface = Saturate((n1 - n0 + heightSlopeOffset) * heightSlopeQ);

		const f32 largeDifferenceNoise = Saturate((1.0f - abs(n0 - n1)) * 1.8f - 0.8f);
		const f32 smallDifferenceNoise = Saturate((1.0f - abs(n2 - n3)) * 1.8f - 0.8f) * isIceSurface;

		const f32 spread = Saturate(position.y / 0.42f);
		const f32 cracks =
			std::pow(largeDifferenceNoise, std::max(1.0f, 61.0f - 60.0f * spread - isCave * 60.0f))
			+ std::pow(smallDifferenceNoise, 27.0f) * 0.332f;

		if (highQuality)
		{
			const f32 n4 = PerlinNoise::GetNoise3D(position * vec3f(131.0f) + vec3f(109.90f, 1289.32f, 3432.032f), octaves - 1u);
			const f32 clouds = (std::pow(n4, 2.7f) + n4 * 0.333f) * isIceSurface;
			return vec3f(Saturate(cracks + clouds * 0.004251f), isIceSurface, 0.0f);
		}
		else
		{
			return vec3f(Saturate(cracks), isIceSurface, 0.0f);
		}
	}
}