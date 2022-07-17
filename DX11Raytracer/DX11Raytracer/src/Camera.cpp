#include "Camera.h"

namespace gfx
{
    Camera::Camera(const vec3 positionWS, const vec3 lookAtWS, const vec3 upVec, const double vFov, const double aspectRatio, const double aperture, const double focusDist)
        : m_positionWS(positionWS)
    {
        const auto theta = DegreesToRadians(vFov);
        const auto halfAngle = std::tan(theta * 0.5);

        m_viewportHeight = 2.0 * halfAngle;
        m_viewportWidth = aspectRatio * m_viewportHeight;

        m_forwardBasis = Normalize(lookAtWS - positionWS);
        m_horizontalBasis = Normalize(Cross(upVec, m_forwardBasis)); // viewportWidth * u * focusDist;
        m_verticalBasis = Cross(m_forwardBasis, m_horizontalBasis); // viewportHeight * v * focusDist;
        m_lowerLeftCorner = positionWS - m_horizontalBasis * (m_viewportWidth * 0.5) - m_verticalBasis * (m_viewportHeight * 0.5) + focusDist * m_forwardBasis;
        m_focusDist = focusDist;

        m_lensRadius = aperture * 0.5;
    }

    Ray Camera::GetRay(double u, double v) const
    {
        if (true)
        {
            const vec3 rd = m_lensRadius * vec3::RandomInUnitDisk();
            const vec3 rayOffset = m_horizontalBasis * rd.x + m_verticalBasis * rd.y;
            const vec3 rayDir = (m_forwardBasis + m_horizontalBasis * (m_viewportWidth * u) + m_verticalBasis * (m_viewportHeight * v)) * m_focusDist - rayOffset;
            return Ray(m_positionWS + rayOffset, rayDir, Random::RandomDouble(), Random::RandomDouble());
        }
        else
        {
            // Normal view with no DOF
            const vec3 rayDir = m_forwardBasis + m_horizontalBasis * (m_viewportWidth * u) + m_verticalBasis * (m_viewportHeight * v);
            return Ray(m_positionWS, rayDir, Random::RandomDouble(), Random::RandomDouble());
        }

        // Use offset to simulate lens at initial position, with ray still pointing to focus point
        vec3 rd = m_lensRadius * vec3::RandomInUnitDisk();
        vec3 offset = u * rd.x + v * rd.y;

        // Time represents open/closed shutter times
        // In this case 0.0 = previous frame and 1.0 = current frame
        vec3 dir = (m_lowerLeftCorner + u * m_horizontalBasis + v * m_verticalBasis - m_positionWS - offset);
    }
}