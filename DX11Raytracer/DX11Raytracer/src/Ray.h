#pragma once
#include "CommonHeader.h"

namespace gfx
{
    class Ray
    {
    public:
        Ray();
        Ray(const vec3& origin, const vec3& direction, const double time, const double randomSeed);
        Ray(const vec3& origin, const vec3& direction, const double time, const double randomSeed, const int pixelIdx);

    public:
        const double GetTime() const;
        const double GetRandomSeed() const;
        const vec3& GetOrigin() const;
        const vec3& GetDirection() const;
        const vec3& GetRcpDirection() const;
        const vec3& GetPositionAfterTime(const double t) const;
        const bool TryGetPixelIdx(const double u, const double v, uint& pixelIdx) const;

    private:
        double m_time;
        double m_randomSeed;
        vec3 m_origin;
        vec3 m_direction;
        vec3 m_rcpDirection;
        int m_pixelIdx;
    };
}