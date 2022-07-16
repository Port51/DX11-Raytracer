#include "RayReceiver.h"
#include "Ray.h"
#include "Material.h"

namespace gfx
{
	RayReceiver::RayReceiver(const vec3 positionWS) : m_positionWS(positionWS)
	{
		m_prevFramePositionWS = m_positionWS + vec3(0, 0.001, 0); // testing motion blur...
	}

	const vec3& RayReceiver::GetCurrentPosition() const
	{
		return m_positionWS;
	}

	const vec3& RayReceiver::GetPreviousFramePosition() const
	{
		return m_prevFramePositionWS;
	}

	const vec3 RayReceiver::GetPositionAtTime(const double time) const
	{
		return m_positionWS * time + m_prevFramePositionWS * (1.0 - time);
	}
}