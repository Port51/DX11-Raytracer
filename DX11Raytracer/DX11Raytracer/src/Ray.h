#pragma once
#include "CommonHeader.h"

namespace gfx
{
    class Ray
    {
    public:
        Ray();
        Ray(const Ray& parent, const vec3f& origin, const vec3f& direction);
        Ray(const vec3f& origin, const vec3f& direction, const f32 time, const f32 randomSeed);
        Ray(const vec3f& origin, const vec3f& direction, const f32 time, const f32 randomSeed, const int pixelIdx);

    public:
        const f32 GetTime() const;
        const f32 GetRandomSeed() const;
        const vec3f& GetOrigin() const;
        const vec3f& GetDirection() const;
        const vec3f& GetRcpDirection() const;
        const vec3f GetPositionAfterTime(const f32 t) const;
        const bool TryGetPixelIdx(const f32 u, const f32 v, uint& pixelIdx) const;

    private:
        void SetRcpDirection();

    private:
        f32 m_time;
        f32 m_randomSeed;
        vec3f m_origin;
        vec3f m_direction;
        vec3f m_rcpDirection;
    public: // temporary
        int m_pixelIdx;
    };
}