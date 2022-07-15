#pragma once
#include "CommonHeader.h"

namespace gfx
{
	class Ray;
	class Material;
	class AABB;

	class RayReceiver
	{
	public:
		RayReceiver() {}
		RayReceiver(const vec3 positionWS) : m_positionWS(positionWS)
		{
			m_prevFramePositionWS = m_positionWS + vec3(0, 0.001, 0); // testing motion blur...
		};
		virtual ~RayReceiver() = default;
	public:
		virtual const bool Hit(const Ray& ray, const double t_min, const double t_max, RayHitRecord& rec) const = 0;
		virtual const bool GetAABB(AABB& aabb) const = 0;
	protected:
		const vec3 GetPositionAtTime(const double time) const
		{
			return m_positionWS * time + m_prevFramePositionWS * (1.0 - time);
		}
	protected:
		vec3 m_positionWS;
		vec3 m_prevFramePositionWS;
	};
}