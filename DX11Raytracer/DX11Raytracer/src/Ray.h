#pragma once
#include "CommonHeader.h"

namespace gfx
{
    class Ray
    {
    public:
        Ray();
        Ray(const vec3& origin, const vec3& direction, const double time = 0.0);

    public:
        const double GetTime() const;
        const vec3& GetOrigin() const;
        const vec3& GetDirection() const;
        const vec3& GetRcpDirection() const;
        const vec3& GetPositionAfterTime(const double t) const;

    private:
        double m_time;
        vec3 m_origin;
        vec3 m_direction;
        vec3 m_rcpDirection;
    };
}