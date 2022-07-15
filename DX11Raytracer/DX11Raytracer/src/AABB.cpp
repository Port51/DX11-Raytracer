#include "AABB.h"

namespace gfx
{
	AABB::AABB() {}

	AABB::AABB(const vec3& a, const vec3& b) : m_minimum(a), m_maximum(b) {}

	const vec3 AABB::GetMinimum() const { return m_minimum; }

	const vec3 AABB::GetMaximum() const { return m_maximum; }

    const bool AABB::Hit(const Ray& r, double t_min, double t_max) const
    {
        vec3 invD = vec3(1.0f / r.GetDirection().x, 1.0f / r.GetDirection().y, 1.0f / r.GetDirection().z);
        double tx1 = (m_minimum.x - r.GetOrigin().x) * invD.x;
        double tx2 = (m_maximum.x - r.GetOrigin().x) * invD.x;
        double tmin = min(tx1, tx2);
        double tmax = max(tx1, tx2);

        double ty1 = (m_minimum.y - r.GetOrigin().y) * invD.y;
        double ty2 = (m_maximum.y - r.GetOrigin().y) * invD.y;
        tmin = max(tmin, min(ty1, ty2));
        tmax = min(tmax, max(ty1, ty2));

        double tz1 = (m_minimum.z - r.GetOrigin().z) * invD.z;
        double tz2 = (m_maximum.z - r.GetOrigin().z) * invD.z;
        tmin = max(tmin, min(tz1, tz2));
        tmax = min(tmax, max(tz1, tz2));

        return tmax >= tmin;
    }

    AABB AABB::GetCombinedAABB(const AABB& box0, const AABB& box1)
    {
        const vec3 minBounds(
            min(box0.GetMinimum().x, box1.GetMinimum().x),
            min(box0.GetMinimum().y, box1.GetMinimum().y),
            min(box0.GetMinimum().z, box1.GetMinimum().z));

        const vec3 maxBounds(
            max(box0.GetMaximum().x, box1.GetMaximum().x),
            max(box0.GetMaximum().y, box1.GetMaximum().y),
            max(box0.GetMaximum().z, box1.GetMaximum().z));

        return AABB(minBounds, maxBounds);
    }
}