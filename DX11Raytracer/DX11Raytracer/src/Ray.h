#pragma once
#include "CommonHeader.h"

namespace gfx
{
    class Ray
    {
    public:
        Ray();
        Ray(const vec3& origin, const vec3& direction);

    public:
        vec3 GetOrigin() const;
        vec3 GetDirection() const;
        vec3 GetPositionAfterTime(const double t) const;

    private:
        vec3 m_origin;
        vec3 m_direction;
    };
}