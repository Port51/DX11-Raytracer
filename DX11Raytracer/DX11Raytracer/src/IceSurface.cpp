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

		double h0 = GetRayHeightAboveSurface(r.GetPositionAfterTime(t0));
		double h1 = GetRayHeightAboveSurface(r.GetPositionAfterTime(t1));

		if (h1 > 0.0) return false;

		// Search for intersection
		double tmid;
		for (uint i = 0u; i < RaymarchHeightFieldTraceSteps; ++i)
		{
			double mix = 0.5;
			//mix = 0.25 + 0.5 * h0 / max(h0 - h1, 0.0001); // bias based on which height is most positive/negative

			tmid = Lerp(t0, t1, mix);
			const vec3& p = r.GetPositionAfterTime(tmid);
			const double& hmid = GetRayHeightAboveSurface(p);

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
			const vec3 normal = GetSurfaceNormal(hitpt);
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
	
	double IceSurface::GetRayHeightAboveSurface(const vec3 p)
	{
		const double radSqr = p.x * p.x + p.z * p.z;
		//const double h = min(1.0, radSqr / 300.0) * std::pow(PerlinNoise::GetNoise3D(p, 2u), 2.0) * 0.2;

		const vec3 sample = IceMaterial::GetIceSample(p, 6u, false);
		const double h =
			(sample.y > 0.0 ? 0.3225 : std::sin(p.x * 5.51) * 0.025);
			//+ (sample.z * 0.271);
		return p.y - h;
	}

	vec3 IceSurface::GetSurfaceNormal(const vec3 p)
	{
		// Lerp between waves and ice surface
		const double c = std::cos(p.x * 5.51) * 0.025;
		const vec3 wavesNormal = -vec3(c, std::sqrt(1.0 - c * c), 0.0);
		const vec3 iceNormal = vec3(0, -1, 0);
		return Lerp(wavesNormal, iceNormal, Saturate(p.y * 100.0));
	}

	const bool IceSurface::GetRayPlaneHit(double& t) const
	{
		return false;
	}
}