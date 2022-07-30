#pragma once
#include "CommonHeader.h"
#include "Ray.h"

namespace gfx
{
    class AABB
    {
    public:
        AABB();
        AABB(const vec3f& a, const vec3f& b);
    public:
        void Set(const vec3f& a, const vec3f& b);
        const vec3f GetMinimum() const;
        const vec3f GetMaximum() const;
        const bool Hit(const Ray& r, const f32 t_min, const f32 t_max) const;

        static AABB GetCombinedAABB(const AABB& box0, const AABB& box1);
    private:
        vec3f m_minimum;
        vec3f m_maximum;
    };
}