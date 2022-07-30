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

		const double iceSurfaceY = Lerp(-1.0, 0.3225, sample.y);
		const double waterSurfaceY = GetWaterLevel(p);

		rayHeightAboveSurface = p.y - max(iceSurfaceY, waterSurfaceY);
		isIceSurface = Saturate((iceSurfaceY - waterSurfaceY) * 1000.0);
	}

	vec3 IceSurface::GetSurfaceNormal(const vec3& p, const float isIceSurface)
	{
		// Ice normals
		const double n0 = PerlinNoise::GetNoise3D(p * vec3(1001.0, 0.0, 1001.0), 3u);
		const double n1 = PerlinNoise::GetNoise3D(p * vec3(1001.0, 0.0, 1001.0) + vec3(0.4289230, 80.239, 324.032), 3u);

		// todo: set this elsewhere!
		const vec3 displ = p - vec3(7.7, 2.05, 2.2);
		const double normalScale = 12.0 + Dot(displ, displ) * 0.11;

		// Prevent artifacts around the horizon
		const vec3 iceNormal = (normalScale < 50.0) ? Normalize(vec3(n0 * 2.0 - 1.0, -normalScale, n1 * 2.0 - 1.0)) : vec3(0, -1, 0);

		if (isIceSurface < 0.999)
		{
			// Lerp to water normals
			const vec3 waterNormal = GetWaterNormal(p);
			return Normalize(Lerp(waterNormal, iceNormal, isIceSurface));
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
		const double n0 = PerlinNoise::GetNoise3D(p * vec3(1251.0, 0.0, 201.0), 3u);
		const double n1 = PerlinNoise::GetNoise3D(p * vec3(1251.0, 0.0, 201.0) + vec3(0.4289230, 80.239, 324.032), 3u);

		return GetWaterLevel(p) + (n0 * n1) * 0.01;
	}

	vec3 IceSurface::GetWaterNormal(const vec3& p)
	{
		const double sampleOffset = 0.01;
		const double p00 = GetWaterLevel_Detailed(p);
		const double p10 = GetWaterLevel_Detailed(p + vec3(sampleOffset, 0, 0));
		const double p01 = GetWaterLevel_Detailed(p + vec3(0, 0, sampleOffset));

		const double dx = (p10 - p00) / sampleOffset;
		const double dz = (p01 - p00) / sampleOffset;

		// todo: set this elsewhere!
		const vec3 displ = p - vec3(7.7, 2.05, 2.2);
		const double normalScale = 1.0 + Dot(displ, displ) * 0.11;

		// Prevent artifacts around the horizon
		return (normalScale < 11.0) ? -Normalize(vec3(dx, normalScale, dz)) : vec3(0, -1, 0);
	}
}