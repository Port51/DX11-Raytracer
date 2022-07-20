#include "WaterSurface.h"

namespace gfx
{
	WaterSurface::WaterSurface(const vec3 positionWS, std::shared_ptr<Material> pMaterial)
		: m_pMaterial(std::move(pMaterial)), RayReceiver(positionWS)
	{}

	const bool WaterSurface::Hit(const Ray & r, const double t_min, const double t_max, RayHitRecord & rec) const
	{
		// Setup initial test pts to only allow possible water heights
		const double limit = 0.2;
		auto t0 = t_min;
		if (r.GetPositionAfterTime(t0).y > limit)
		{
			t0 = (limit - r.GetOrigin().y) / r.GetDirection().y;
		}

		auto t1 = min(1000.0, t_max);
		if (r.GetPositionAfterTime(t1).y < 0.0)
		{
			t1 = (0.0 - r.GetOrigin().y) / r.GetDirection().y;
		}

		auto h0 = GetRayHeightAboveOcean(r.GetPositionAfterTime(t0));
		auto h1 = GetRayHeightAboveOcean(r.GetPositionAfterTime(t1));

		if (h1 > 0.0) return false;

		// Search for intersection
		double tmid;
		for (uint i = 0u; i < 10u; ++i)
		{
			auto mix = 0.5;
			mix = h0 / max(h0 - h1, 0.0001); // bias based on which height is most positive/negative

			tmid = t0 * (1.0 - mix) + t1 * mix;
			auto p = r.GetPositionAfterTime(tmid);
			auto hmid = GetRayHeightAboveOcean(p);

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
			const vec3 normal = GetOceanNormal(hitpt);
			rec.time = tmid;
			rec.isFrontFacing = Dot(r.GetDirection(), normal) < 0;
			rec.normalWS = normal;
			rec.positionWS = hitpt;
			rec.pMaterial = m_pMaterial;
			rec.u = rec.positionWS.x;
			rec.v = rec.positionWS.z;
			return true;
		}
		else
		{
			return false;
		}

		/*
		// Ray-plane intersection
		// t = (P0 - R0) * P / (R * P)
		const vec3 normal = vec3(0.0, 1.0, 0.0);
		const auto t = Dot(m_positionWS - r.GetOrigin(), normal) / Dot(r.GetDirection(), normal);
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
		}
		*/
	}

	const bool WaterSurface::GetAABB(AABB& aabb) const
	{
		const double size = 100.0;
		const double depth = 0.1;
		aabb.Set(vec3(-size, -depth, -size), vec3(size, depth, size));
		return true;
	}
	
	double WaterSurface::GetRayHeightAboveOcean(const vec3 p)
	{
		return p.y - (std::sin(p.x * 10.0) * 0.1 + 0.1);
	}

	vec3 WaterSurface::GetOceanNormal(const vec3 p)
	{
		return vec3(std::cos(p.x * 10.0), std::sin(p.x * 10.0), 0.0);
	}

	const bool WaterSurface::GetRayPlaneHit(double& t) const
	{
		return false;
	}
}