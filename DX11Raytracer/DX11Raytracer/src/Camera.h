#pragma once
#include "CommonHeader.h"
#include "Ray.h"

namespace gfx
{
    class Camera
    {
    public:
        Camera(const vec3 positionWS, const vec3 lookAtWS, const vec3 upVec, const double vFov, const double aspectRatio, const double aperture, const double focusDist);
    public:
        Ray GetRay(double u, double v) const;
    private:
        vec3 m_positionWS;
        vec3 m_horizontalBasis;
        vec3 m_verticalBasis;
        vec3 m_lowerLeftCorner;
        vec3 w, u, v;
        double m_lensRadius;
    };
}