#include "IceSurface.h"
#include "PerlinNoise.h"
#include "Material.h"
#include "IceMaterial.h"
#include "Settings.h"

namespace gfx
{
	IceSurface::IceSurface(const vec3f positionWS, std::shared_ptr<Material> pMaterial)
		: m_pMaterial(std::move(pMaterial)), RayReceiver(positionWS)
	{}

	const bool IceSurface::Hit(const Ray & r, const f32 t_min, const f32 t_max, RayHitRecord & rec, const uint gBufferIdx) const
	{
		// Filter by GBuffer
		if (!m_pMaterial->IsInGBuffer(gBufferIdx)) return false;

		// Setup initial test pts to only allow possible water heights
		const f32 limit = 0.35f;
		f32 t0 = t_min;
		if (r.GetPositionAfterTime(t0).y > limit)
		{
			t0 = (limit - r.GetOrigin().y) / r.GetDirection().y;
		}

		f32 t1 = std::min(1000.f, t_max);
		if (r.GetPositionAfterTime(t1).y < -0.05f)
		{
			t1 = (-0.05f - r.GetOrigin().y) / r.GetDirection().y;
		}

		f32 h0, h1;
		f32 isIceSurface;
		GetSurfaceInfo(r.GetPositionAfterTime(t0), h0, isIceSurface);
		GetSurfaceInfo(r.GetPositionAfterTime(t1), h1, isIceSurface);

		if (h1 > 0.0f) return false;

		// Search for intersection
		f32 tmid;
		for (uint i = 0u; i < RaymarchHeightFieldTraceSteps; ++i)
		{
			f32 mix = 0.5f;
			//mix = 0.25 + 0.5 * h0 / std::max(h0 - h1, 0.0001); // bias based on which height is most positive/negative

			tmid = Lerp(t0, t1, mix);
			const vec3f& p = r.GetPositionAfterTime(tmid);
			f32 hmid;
			GetSurfaceInfo(p, hmid, isIceSurface);

			if (hmid < 0.0f)
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
			const vec3f hitpt = r.GetPositionAfterTime(tmid);
			const vec3f normal = GetSurfaceNormal(hitpt, isIceSurface);
			rec.time = tmid;
			rec.isFrontFacing = Dot(r.GetDirection(), normal) < 0.f;
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
		/*const vec3f normal = vec3f(0.0, -1.0, 0.0);
		const f32 t = Dot(m_positionWS - r.GetOrigin(), normal) / Dot(r.GetDirection(), normal);
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
		const f32 size = 1000000.0f;
		const f32 depth = 0.1f;
		aabb.Set(vec3f(-size, -depth, -size), vec3f(size, depth, size));
		return true;
	}
	
	void IceSurface::GetSurfaceInfo(const vec3f& p, f32& rayHeightAboveSurface, f32& isIceSurface) const
	{
		const vec3f sample = IceMaterial::GetIceSample(p, 6u, false);

		const f32 iceSurfaceY = Lerp(-1.0f, 0.3225f, sample.y);
		const f32 waterSurfaceY = GetWaterLevel(p);

		rayHeightAboveSurface = p.y - std::max(iceSurfaceY, waterSurfaceY);
		isIceSurface = Saturate((iceSurfaceY - waterSurfaceY) * 1000.0f);
	}

	vec3f IceSurface::GetSurfaceNormal(const vec3f& p, const f32 isIceSurface)
	{
		// Ice normals
		const f32 n0 = PerlinNoise::GetNoise3D(p * vec3f(1001.0f, 0.0f, 1001.0f), 3u);
		const f32 n1 = PerlinNoise::GetNoise3D(p * vec3f(1001.0f, 0.0f, 1001.0f) + vec3f(0.4289230f, 80.239f, 324.032f), 3u);

		// todo: set this elsewhere!
		const vec3f displ = p - vec3f(7.7f, 2.05f, 2.2f);
		const f32 normalScale = 12.0f + Dot(displ, displ) * 0.11f;

		// Prevent artifacts around the horizon
		const vec3f iceNormal = (normalScale < 50.0f) ? Normalize(vec3f(n0 * 2.0f - 1.0f, -normalScale, n1 * 2.0f - 1.0f)) : vec3f(0.f, -1.f, 0.f);

		if (isIceSurface < 0.999f)
		{
			// Lerp to water normals
			const vec3f waterNormal = GetWaterNormal(p);
			return Normalize(Lerp(waterNormal, iceNormal, isIceSurface));
		}
		return iceNormal;
	}

	const bool IceSurface::GetRayPlaneHit(f32& t) const
	{
		return false;
	}

	f32 IceSurface::GetWaterLevel(const vec3f& p)
	{
		return std::sin(p.x * 5.51f) * 0.01551f;
	}

	f32 IceSurface::GetWaterLevel_Detailed(const vec3f& p)
	{
		const f32 n0 = PerlinNoise::GetNoise3D(p * vec3f(1251.0f, 0.0f, 201.0f), 3u);
		const f32 n1 = PerlinNoise::GetNoise3D(p * vec3f(1251.0f, 0.0f, 201.0f) + vec3f(0.4289230f, 80.239f, 324.032f), 3u);

		return GetWaterLevel(p) + (n0 * n1) * 0.01f;
	}

	vec3f IceSurface::GetWaterNormal(const vec3f& p)
	{
		const f32 sampleOffset = 0.01f;
		const f32 p00 = GetWaterLevel_Detailed(p);
		const f32 p10 = GetWaterLevel_Detailed(p + vec3f(sampleOffset, 0.f, 0.f));
		const f32 p01 = GetWaterLevel_Detailed(p + vec3f(0.f, 0.f, sampleOffset));

		const f32 dx = (p10 - p00) / sampleOffset;
		const f32 dz = (p01 - p00) / sampleOffset;

		// todo: set this elsewhere!
		const vec3f displ = p - vec3f(7.7f, 2.05f, 2.2f);
		const f32 normalScale = 1.0f + Dot(displ, displ) * 0.11f;

		// Prevent artifacts around the horizon
		return (normalScale < 11.0f) ? -Normalize(vec3f(dx, normalScale, dz)) : vec3f(0.f, -1.f, 0.f);
	}
}