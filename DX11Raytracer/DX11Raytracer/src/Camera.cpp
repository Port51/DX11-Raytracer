#include "Camera.h"

namespace gfx
{
    Camera::Camera(const vec3 positionWS, const vec3 lookAtWS, const vec3 upVec, const double vFov, const double aspectRatio, const double aperture, const double focusDist)
        : m_positionWS(positionWS)
    {
        const auto theta = DegreesToRadians(vFov);
        const auto halfAngle = std::tan(theta * 0.5);
        const auto viewportHeight = 2.0 * halfAngle;
        const auto viewportWidth = aspectRatio * viewportHeight;

        w = Normalize(positionWS - lookAtWS);
        u = Normalize(Cross(upVec, w));
        v = Cross(w, u);

        m_horizontalBasis = viewportWidth * u * focusDist;
        m_verticalBasis = viewportHeight * v * focusDist;
        m_lowerLeftCorner = positionWS - m_horizontalBasis * 0.5 - m_verticalBasis * 0.5 - focusDist * w;

        m_lensRadius = aperture * 0.5;
    }

    Ray Camera::GetRay(double u, double v) const
    {
        // Use offset to simulate lens at initial position, with ray still pointing to focus point
        vec3 rd = m_lensRadius * vec3::RandomInUnitDisk();
        vec3 offset = u * rd.x + v * rd.y;

        // Time represents open/closed shutter times
        // In this case 0.0 = previous frame and 1.0 = current frame
        vec3 dir = Normalize(m_lowerLeftCorner + u * m_horizontalBasis + v * m_verticalBasis - m_positionWS - offset);
        return Ray(m_positionWS + offset, dir, Random::RandomDouble());
    }
}