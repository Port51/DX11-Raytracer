#include "AABB.h"

namespace gfx
{
	AABB::AABB() {}

	AABB::AABB(const vec3f& a, const vec3f& b) : m_minimum(a), m_maximum(b) {}

    void AABB::Set(const vec3f& a, const vec3f& b)
    {
        m_minimum = a;
        m_maximum = b;
    }

    const vec3f AABB::GetMinimum() const { return m_minimum; }

	const vec3f AABB::GetMaximum() const { return m_maximum; }

    const bool AABB::Hit(const Ray& r, const f32 t_min, const f32 t_max) const
    {
        const vec3f& rayOrigin = r.GetOrigin();
        const vec3f& rayRcpDir = r.GetRcpDirection();

        const f32 tx1 = (m_minimum.x - rayOrigin.x) * rayRcpDir.x;
        const f32 tx2 = (m_maximum.x - rayOrigin.x) * rayRcpDir.x;
        f32 tmin = std::min(tx1, tx2);
        f32 tmax = std::max(tx1, tx2);

        const f32 ty1 = (m_minimum.y - rayOrigin.y) * rayRcpDir.y;
        const f32 ty2 = (m_maximum.y - rayOrigin.y) * rayRcpDir.y;
        tmin = std::max(tmin, std::min(ty1, ty2));
        tmax = std::min(tmax, std::max(ty1, ty2));

        const f32 tz1 = (m_minimum.z - rayOrigin.z) * rayRcpDir.z;
        const f32 tz2 = (m_maximum.z - rayOrigin.z) * rayRcpDir.z;
        tmin = std::max(tmin, std::min(tz1, tz2));
        tmax = std::min(tmax, std::max(tz1, tz2));

        return (tmax >= tmin);
    }

    AABB AABB::GetCombinedAABB(const AABB& box0, const AABB& box1)
    {
        const vec3f minBounds(
            std::min(box0.GetMinimum().x, box1.GetMinimum().x),
            std::min(box0.GetMinimum().y, box1.GetMinimum().y),
            std::min(box0.GetMinimum().z, box1.GetMinimum().z));

        const vec3f maxBounds(
            std::max(box0.GetMaximum().x, box1.GetMaximum().x),
            std::max(box0.GetMaximum().y, box1.GetMaximum().y),
            std::max(box0.GetMaximum().z, box1.GetMaximum().z));

        return AABB(minBounds, maxBounds);
    }
}