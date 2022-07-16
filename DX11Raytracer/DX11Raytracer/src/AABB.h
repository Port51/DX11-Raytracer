#pragma once
#include "CommonHeader.h"
#include "Ray.h"

namespace gfx
{
    class AABB
    {
    public:
        AABB();
        AABB(const vec3& a, const vec3& b);
    public:
        const vec3 GetMinimum() const;
        const vec3 GetMaximum() const;
        const bool Hit(const Ray& r, const double t_min, const double t_max) const;

        static AABB GetCombinedAABB(const AABB& box0, const AABB& box1);
    private:
        vec3 m_minimum;
        vec3 m_maximum;
    };
}