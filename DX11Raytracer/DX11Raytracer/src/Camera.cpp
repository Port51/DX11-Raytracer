#include "Camera.h"
#include "Settings.h"

namespace gfx
{
    Camera::Camera(const vec3f positionWS, const vec3f lookAtWS, const vec3f upVec, const f32 vFov, const f32 aspectRatio, const f32 aperture, const f32 focusDist)
        : m_positionWS(positionWS)
    {
        const f32 theta = DegreesToRadians(vFov);
        const f32 halfAngle = std::tan(theta * 0.5f);

        m_viewportHeight = 2.0f * halfAngle;
        m_viewportWidth = aspectRatio * m_viewportHeight;

        m_forwardBasis = Normalize(lookAtWS - positionWS);
        m_horizontalBasis = Normalize(Cross(upVec, m_forwardBasis)); // viewportWidth * u * focusDist;
        m_verticalBasis = Cross(m_forwardBasis, m_horizontalBasis); // viewportHeight * v * focusDist;
        m_focusDist = focusDist;

        m_lensRadius = aperture * 0.5f;
    }

    Ray Camera::GetRay(const f32 u, const f32 v, const uint pixelIdx, const bool useDepthOfField) const
    {
        if (useDepthOfField)
        {
            // Offset rays by disc
            const vec3f discOffset = m_lensRadius * vec3f::RandomInUnitDisk();
            const vec3f rayOffset = m_horizontalBasis * discOffset.x + m_verticalBasis * discOffset.y;
            const vec3f rayDir = (m_forwardBasis + m_horizontalBasis * (m_viewportWidth * u) + m_verticalBasis * (m_viewportHeight * v)) * m_focusDist - rayOffset;
            return Ray(m_positionWS + rayOffset, rayDir, Random::RandomFloat(), Random::RandomFloat(), pixelIdx);
        }
        else
        {
            // Normal ray with no DOF
            const vec3f rayDir = m_forwardBasis + m_horizontalBasis * (m_viewportWidth * u) + m_verticalBasis * (m_viewportHeight * v);
            return Ray(m_positionWS, rayDir, Random::RandomFloat(), Random::RandomFloat(), pixelIdx);
        }
    }
}