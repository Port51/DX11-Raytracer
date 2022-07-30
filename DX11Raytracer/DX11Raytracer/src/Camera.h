#pragma once
#include "CommonHeader.h"
#include "Ray.h"

namespace gfx
{
    class Camera
    {
    public:
        Camera(const vec3f positionWS, const vec3f lookAtWS, const vec3f upVec, const f32 vFov, const f32 aspectRatio, const f32 aperture, const f32 focusDist);
    public:
        Ray GetRay(const f32 u, const f32 v, const uint pixelIdx, const bool useDepthOfField) const;
    private:
        vec3f m_positionWS;
        vec3f m_horizontalBasis;
        vec3f m_verticalBasis;
        vec3f m_forwardBasis;
        f32 m_viewportWidth;
        f32 m_viewportHeight;
        f32 m_lensRadius;
        f32 m_focusDist;
    };
}