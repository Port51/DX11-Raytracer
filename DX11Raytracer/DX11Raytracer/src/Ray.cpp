#include "Ray.h"
#include "Settings.h"

namespace gfx
{

	Ray::Ray()
    {}

    Ray::Ray(const Ray & parent, const vec3f& origin, const vec3f & direction)
        : m_origin(origin), m_direction(Normalize(direction)), m_time(parent.m_time), m_randomSeed(parent.m_randomSeed), m_pixelIdx(-1)
    {
        SetRcpDirection();
    }

    Ray::Ray(const vec3f & origin, const vec3f & direction, const f32 time, const f32 randomSeed)
        : m_origin(origin), m_direction(Normalize(direction)), m_time(time), m_randomSeed(randomSeed), m_pixelIdx(-1)
    {
        SetRcpDirection();
    }

    Ray::Ray(const vec3f& origin, const vec3f& direction, const f32 time, const f32 randomSeed, const int pixelIdx)
        : m_origin(origin), m_direction(Normalize(direction)), m_time(time), m_randomSeed(randomSeed), m_pixelIdx(pixelIdx)
    {
        SetRcpDirection();
    }

    const f32 Ray::GetTime() const
    {
        return m_time;
    }

    const f32 Ray::GetRandomSeed() const
    {
        return m_randomSeed;
    }

    const vec3f& Ray::GetOrigin() const
    {
        return m_origin;
    }

    const vec3f& Ray::GetDirection() const
    {
        return m_direction;
    }

    const vec3f& Ray::GetRcpDirection() const
    {
        return m_rcpDirection;
    }

    const vec3f Ray::GetPositionAfterTime(const f32 t) const
    {
        return m_origin + t * m_direction;
    }

    const bool Ray::TryGetPixelIdx(const f32 u, const f32 v, uint& pixelIdx) const
    {
        if (m_pixelIdx != -1)
        {
            pixelIdx = m_pixelIdx;
            return true;
        }
        return false;

        // Reconstruct pixel index from UVs
        /*if (m_pixelIdx != -1)
        {
            int x = u * ScreenWidth;
            int y = v * ScreenHeight;

            if (x < 0 || x >= ScreenWidth || y < 0 || y >= ScreenHeight) return false;

            pixelIdx = y * ScreenWidth + x;
            return true;
        }
        return false;*/
    }

    void Ray::SetRcpDirection()
    {
        m_rcpDirection = vec3f(1.0f / m_direction.x, 1.0f / m_direction.y, 1.0f / m_direction.z);
    }

}