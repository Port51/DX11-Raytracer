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
        Ray GetRay(const double u, const double v, const uint pixelIdx) const;
    private:
        vec3 m_positionWS;
        vec3 m_horizontalBasis;
        vec3 m_verticalBasis;
        vec3 m_forwardBasis;
        double m_viewportWidth;
        double m_viewportHeight;
        double m_lensRadius;
        double m_focusDist;
    };
}