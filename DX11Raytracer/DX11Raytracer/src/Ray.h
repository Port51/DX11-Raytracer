#pragma once
#include "CommonHeader.h"

namespace gfx
{
    class Ray
    {
    public:
        Ray() {}
        Ray(const vec3& origin, const vec3& direction)
            : m_origin(origin), m_direction(direction)
        {}

    public:
        vec3 GetOrigin() const { return m_origin; }
        vec3 GetDirection() const { return m_direction; }
        vec3 GetPositionAfterTime(const double t) const
        {
            return m_origin + t * m_direction;
        }

    private:
        vec3 m_origin;
        vec3 m_direction;
    };
}