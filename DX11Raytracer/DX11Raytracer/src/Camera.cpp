#include "Camera.h"
#include "Settings.h"

namespace gfx
{
    Camera::Camera(const vec3 positionWS, const vec3 lookAtWS, const vec3 upVec, const double vFov, const double aspectRatio, const double aperture, const double focusDist)
        : m_positionWS(positionWS)
    {
        const double theta = DegreesToRadians(vFov);
        const double halfAngle = std::tan(theta * 0.5);

        m_viewportHeight = 2.0 * halfAngle;
        m_viewportWidth = aspectRatio * m_viewportHeight;

        m_forwardBasis = Normalize(lookAtWS - positionWS);
        m_horizontalBasis = Normalize(Cross(upVec, m_forwardBasis)); // viewportWidth * u * focusDist;
        m_verticalBasis = Cross(m_forwardBasis, m_horizontalBasis); // viewportHeight * v * focusDist;
        m_focusDist = focusDist;

        m_lensRadius = aperture * 0.5;
    }

    Ray Camera::GetRay(const double u, const double v, const uint pixelIdx, const bool useDepthOfField) const
    {
        if (useDepthOfField)
        {
            // Offset rays by disc
            const vec3 discOffset = m_lensRadius * vec3::RandomInUnitDisk();
            const vec3 rayOffset = m_horizontalBasis * discOffset.x + m_verticalBasis * discOffset.y;
            const vec3 rayDir = (m_forwardBasis + m_horizontalBasis * (m_viewportWidth * u) + m_verticalBasis * (m_viewportHeight * v)) * m_focusDist - rayOffset;
            return Ray(m_positionWS + rayOffset, rayDir, Random::RandomDouble(), Random::RandomDouble(), pixelIdx);
        }
        else
        {
            // Normal ray with no DOF
            const vec3 rayDir = m_forwardBasis + m_horizontalBasis * (m_viewportWidth * u) + m_verticalBasis * (m_viewportHeight * v);
            return Ray(m_positionWS, rayDir, Random::RandomDouble(), Random::RandomDouble(), pixelIdx);
        }
    }
}