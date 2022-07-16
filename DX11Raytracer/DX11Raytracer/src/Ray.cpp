#include "Ray.h"

namespace gfx
{

	Ray::Ray()
    {}

    Ray::Ray(const vec3& origin, const vec3& direction, const double time)
        : m_origin(origin), m_direction(direction), m_rcpDirection(vec3(1.0 / direction.x, 1.0 / direction.y, 1.0 / direction.z)), m_time(time)
    {}

    const double Ray::GetTime() const
    {
        return m_time;
    }

    const vec3& Ray::GetOrigin() const
    {
        return m_origin;
    }

    const vec3& Ray::GetDirection() const
    {
        return m_direction;
    }

    const vec3& Ray::GetRcpDirection() const
    {
        return m_rcpDirection;
    }

    const vec3& Ray::GetPositionAfterTime(const double t) const
    {
        return m_origin + t * m_direction;
    }

}