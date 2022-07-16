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
		RayReceiver() = default;
		RayReceiver(const vec3 positionWS);;
		virtual ~RayReceiver() = default;
	public:
		virtual const bool Hit(const Ray& ray, const double t_min, const double t_max, RayHitRecord& rec) const = 0;
		virtual const bool GetAABB(AABB& aabb) const = 0;
		const vec3& GetCurrentPosition() const;
		const vec3& GetPreviousFramePosition() const;
	protected:
		const vec3 GetPositionAtTime(const double time) const;
	protected:
		vec3 m_positionWS;
		vec3 m_prevFramePositionWS;
	};
}