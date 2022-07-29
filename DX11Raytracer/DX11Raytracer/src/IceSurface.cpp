#include "IceSurface.h"
#include "PerlinNoise.h"
#include "Material.h"
#include "IceMaterial.h"
#include "Settings.h"

namespace gfx
{
	IceSurface::IceSurface(const vec3 positionWS, std::shared_ptr<Material> pMaterial)
		: m_pMaterial(std::move(pMaterial)), RayReceiver(positionWS)
	{}

	const bool IceSurface::Hit(const Ray & r, const double t_min, const double t_max, RayHitRecord & rec, const uint gBufferIdx) const
	{
		// Filter by GBuffer
		if (!m_pMaterial->IsInGBuffer(gBufferIdx)) return false;

		// Setup initial test pts to only allow possible water heights
		const double limit = 0.35;
		double t0 = t_min;
		if (r.GetPositionAfterTime(t0).y > limit)
		{
			t0 = (limit - r.GetOrigin().y) / r.GetDirection().y;
		}

		double t1 = min(1000.0, t_max);
		if (r.GetPositionAfterTime(t1).y < -0.05)
		{
			t1 = (-0.05 - r.GetOrigin().y) / r.GetDirection().y;
		}

		double h0, h1;
		float isIceSurface;
		GetSurfaceInfo(r.GetPositionAfterTime(t0), h0, isIceSurface);
		GetSurfaceInfo(r.GetPositionAfterTime(t1), h1, isIceSurface);

		if (h1 > 0.0) return false;

		// Search for intersection
		double tmid;
		for (uint i = 0u; i < RaymarchHeightFieldTraceSteps; ++i)
		{
			double mix = 0.5;
			//mix = 0.25 + 0.5 * h0 / max(h0 - h1, 0.0001); // bias based on which height is most positive/negative

			tmid = Lerp(t0, t1, mix);
			const vec3& p = r.GetPositionAfterTime(tmid);
			double hmid;
			GetSurfaceInfo(p, hmid, isIceSurface);

			if (hmid < 0.0)
			{
				t1 = tmid;
				h1 = hmid;
			}
			else
			{
				t0 = tmid;
				h0 = hmid;
			}
		}

		// Create intersection
		if (tmid >= t_min && tmid <= t_max)
		{
			const vec3 hitpt = r.GetPositionAfterTime(tmid);
			const vec3 normal = GetSurfaceNormal(hitpt, isIceSurface);
			rec.time = tmid;
			rec.isFrontFacing = Dot(r.GetDirection(), normal) < 0;
			rec.normalWS = normal;
			rec.positionWS = hitpt;
			rec.pMaterial = m_pMaterial;
			rec.materialSubIndex = isIceSurface;
			rec.u = rec.positionWS.x;
			rec.v = rec.positionWS.z;
			return true;
		}
		else
		{
			return false;
		}
		
		// Ray-plane intersection
		// t = (P0 - R0) * P / (R * P)
		/*const vec3 normal = vec3(0.0, -1.0, 0.0);
		const double t = Dot(m_positionWS - r.GetOrigin(), normal) / Dot(r.GetDirection(), normal);
		if (t >= t_min && t <= t_max)
		{
			rec.time = t;
			rec.isFrontFacing = Dot(r.GetDirection(), normal) < 0;
			rec.normalWS = normal;
			rec.positionWS = r.GetPositionAfterTime(t);
			rec.pMaterial = m_pMaterial;
			rec.u = rec.positionWS.x;
			rec.v = rec.positionWS.z;
			return true;
		}
		else
		{
			return false;
		}*/
	}

	const bool IceSurface::GetAABB(AABB& aabb) const
	{
		const double size = 1000000.0;
		const double depth = 0.1;
		aabb.Set(vec3(-size, -depth, -size), vec3(size, depth, size));
		return true;
	}
	
	void IceSurface::GetSurfaceInfo(const vec3& p, double& rayHeightAboveSurface, float& isIceSurface) const
	{
		const vec3 sample = IceMaterial::GetIceSample(p, 6u, false);
		isIceSurface = static_cast<float>(Saturate(sample.y));

		const double iceSurfaceY = 0.3225;
		const double waterSurfaceY = GetWaterLevel(p);

		const double surfaceY = Lerp(waterSurfaceY, iceSurfaceY, static_cast<double>(isIceSurface));
		rayHeightAboveSurface = p.y - surfaceY;
	}

	vec3 IceSurface::GetSurfaceNormal(const vec3& p, const bool isIceSurface)
	{
		// Lerp between waves and ice surface
		const double c = std::cos(p.x * 5.51) * 0.025;
		const vec3 iceNormal = vec3(0, -1, 0);
		const double lerp = Saturate(p.y * 100.0);

		if (lerp < 0.999)
		{
			const vec3 waterNormal = GetWaterNormal(p);
			return Lerp(waterNormal, iceNormal, lerp);
		}
		return iceNormal;
	}

	const bool IceSurface::GetRayPlaneHit(double& t) const
	{
		return false;
	}

	double IceSurface::GetWaterLevel(const vec3& p)
	{
		return std::sin(p.x * 5.51) * 0.00551;
	}

	double IceSurface::GetWaterLevel_Detailed(const vec3& p)
	{
		const double n0 = PerlinNoise::GetNoise3D(p * vec3(1551.0, 1.0, 1551.0), 4u);
		const double n1 = PerlinNoise::GetNoise3D(p * vec3(1551.0, 1.0, 1551.0) + vec3(0.4289230, 80.239, 324.032), 4u);

		return GetWaterLevel(p) + (n0 * n1) * 0.05;
	}

	vec3 IceSurface::GetWaterNormal(const vec3& p)
	{
		const double sampleOffset = 0.001;
		const double p00 = GetWaterLevel_Detailed(p);
		const double p10 = GetWaterLevel_Detailed(p + vec3(sampleOffset, 0, 0));
		const double p01 = GetWaterLevel_Detailed(p + vec3(0, 0, sampleOffset));

		const double dx = (p10 - p00) / sampleOffset;
		const double dz = (p01 - p00) / sampleOffset;
		return -Normalize(vec3(dx, 1.0, dz));
	}
}