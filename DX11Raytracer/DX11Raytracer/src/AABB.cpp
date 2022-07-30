#include "AABB.h"

namespace gfx
{
	AABB::AABB() {}

	AABB::AABB(const vec3& a, const vec3& b) : m_minimum(a), m_maximum(b) {}

    void AABB::Set(const vec3& a, const vec3& b)
    {
        m_minimum = a;
        m_maximum = b;
    }

    const vec3 AABB::GetMinimum() const { return m_minimum; }

	const vec3 AABB::GetMaximum() const { return m_maximum; }

    const bool AABB::Hit(const Ray& r, const double t_min, const double t_max) const
    {
        const vec3& rayOrigin = r.GetOrigin();
        const vec3& rayRcpDir = r.GetRcpDirection();

        const double tx1 = (m_minimum.x - rayOrigin.x) * rayRcpDir.x;
        const double tx2 = (m_maximum.x - rayOrigin.x) * rayRcpDir.x;
        double tmin = std::min(tx1, tx2);
        double tmax = std::max(tx1, tx2);

        const double ty1 = (m_minimum.y - rayOrigin.y) * rayRcpDir.y;
        const double ty2 = (m_maximum.y - rayOrigin.y) * rayRcpDir.y;
        tmin = std::max(tmin, std::min(ty1, ty2));
        tmax = std::min(tmax, std::max(ty1, ty2));

        const double tz1 = (m_minimum.z - rayOrigin.z) * rayRcpDir.z;
        const double tz2 = (m_maximum.z - rayOrigin.z) * rayRcpDir.z;
        tmin = std::max(tmin, std::min(tz1, tz2));
        tmax = std::min(tmax, std::max(tz1, tz2));

        return (tmax >= tmin);
    }

    AABB AABB::GetCombinedAABB(const AABB& box0, const AABB& box1)
    {
        const vec3 minBounds(
            std::min(box0.GetMinimum().x, box1.GetMinimum().x),
            std::min(box0.GetMinimum().y, box1.GetMinimum().y),
            std::min(box0.GetMinimum().z, box1.GetMinimum().z));

        const vec3 maxBounds(
            std::max(box0.GetMaximum().x, box1.GetMaximum().x),
            std::max(box0.GetMaximum().y, box1.GetMaximum().y),
            std::max(box0.GetMaximum().z, box1.GetMaximum().z));

        return AABB(minBounds, maxBounds);
    }
}