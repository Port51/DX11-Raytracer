#include "Ray.h"

namespace gfx
{

	Ray::Ray()
    {}

    Ray::Ray(const vec3& origin, const vec3& direction, const double time, const double randomSeed, const uint pixelIdx)
        : m_origin(origin), m_direction(direction), m_rcpDirection(vec3(1.0 / direction.x, 1.0 / direction.y, 1.0 / direction.z)), m_time(time), m_randomSeed(randomSeed), m_pixelIdx(pixelIdx)
    {}

    const double Ray::GetTime() const
    {
        return m_time;
    }

    const double Ray::GetRandomSeed() const
    {
        return m_randomSeed;
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

    const uint Ray::GetPixelIdx() const
    {
        return m_pixelIdx;
    }

}