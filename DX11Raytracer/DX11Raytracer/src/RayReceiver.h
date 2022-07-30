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
		RayReceiver(const vec3f positionWS);;
		virtual ~RayReceiver() = default;
	public:
		virtual const bool Hit(const Ray& ray, const f32 t_min, const f32 t_max, RayHitRecord& rec, const uint gBufferIdx) const = 0;
		virtual const bool GetAABB(AABB& aabb) const = 0;
		const vec3f& GetCurrentPosition() const;
		const vec3f& GetPreviousFramePosition() const;
	protected:
		const vec3f GetPositionAtTime(const f32 time) const;
	protected:
		vec3f m_positionWS;
		vec3f m_prevFramePositionWS;
	};
}