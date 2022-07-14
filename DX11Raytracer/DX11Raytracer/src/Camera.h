#pragma once
#include "CommonHeader.h"
#include "Ray.h"

namespace gfx
{
    class Camera
    {
    public:
        Camera(const vec3 positionWS, const vec3 lookAtWS, const vec3 upVec, const double vFov, const double aspect_ratio)
            : m_positionWS(positionWS)
        {
            auto theta = degrees_to_radians(vFov);
            auto halfAngle = tan(theta * 0.5);
            auto viewportHeight = 2.0 * halfAngle;
            auto viewportWidth = aspect_ratio * viewportHeight;

            auto w = Normalize(positionWS - lookAtWS);
            auto u = Normalize(Cross(upVec, w));
            auto v = Cross(w, u);

            m_horizontalBasis = viewportWidth * u;
            m_verticalBasis = viewportHeight * v;
            m_lowerLeftCorner = positionWS - m_horizontalBasis * 0.5 - m_verticalBasis * 0.5 - w;

        }

        Ray GetRay(double u, double v) const
        {
            vec3 dir = Normalize(m_lowerLeftCorner + u * m_horizontalBasis + v * m_verticalBasis - m_positionWS);
            return Ray(m_positionWS, dir);
        }

    private:
        vec3 m_positionWS;
        vec3 m_horizontalBasis;
        vec3 m_verticalBasis;
        vec3 m_lowerLeftCorner;
    };
}